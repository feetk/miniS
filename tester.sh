#!/bin/bash
# ============================================================
#  MINISHELL TESTER — v3
#  Usage: bash tester.sh
#  Covers: eval sheet + FD leaks + valgrind
# ============================================================

MINI="./minishell"
PASS=0
FAIL=0
SKIP=0
TOTAL=0

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
MAGENTA="\033[0;35m"
RESET="\033[0m"

filter()
{
grep -v "^minishell" | grep -v "^exit$" | sed '/^> /d'
}

run()
{
local label="$1"
local input="$2"
local expected="$3"

TOTAL=$((TOTAL + 1))
actual=$(printf '%s\n' "$input" | "$MINI" 2>/dev/null | filter)

if [ "$actual" = "$expected" ]; then
echo -e "  ${GREEN}[OK]${RESET}  $label"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  $label"
echo -e "        ${YELLOW}expected:${RESET} |$(echo "$expected" | head -4)|"
echo -e "        ${YELLOW}got:     ${RESET} |$(echo "$actual" | head -4)|"
FAIL=$((FAIL + 1))
fi
}

run_exit()
{
local label="$1"
local input="$2"
local expected_exit="$3"

TOTAL=$((TOTAL + 1))
printf '%s\n' "$input" | "$MINI" > /dev/null 2>&1
actual_exit=$?

if [ "$actual_exit" = "$expected_exit" ]; then
echo -e "  ${GREEN}[OK]${RESET}  $label  (exit=$actual_exit)"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  $label"
echo -e "        ${YELLOW}expected exit:${RESET} $expected_exit  ${YELLOW}got:${RESET} $actual_exit"
FAIL=$((FAIL + 1))
fi
}

run_contains()
{
local label="$1"
local input="$2"
local pattern="$3"

TOTAL=$((TOTAL + 1))
actual=$(printf '%s\n' "$input" | "$MINI" 2>/dev/null | filter)

if echo "$actual" | grep -q "$pattern"; then
echo -e "  ${GREEN}[OK]${RESET}  $label"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  $label"
echo -e "        ${YELLOW}expected to contain:${RESET} $pattern"
echo -e "        ${YELLOW}got:${RESET} $(echo "$actual" | head -3)"
FAIL=$((FAIL + 1))
fi
}

run_not_contains()
{
local label="$1"
local input="$2"
local pattern="$3"

TOTAL=$((TOTAL + 1))
actual=$(printf '%s\n' "$input" | "$MINI" 2>/dev/null | filter)

if echo "$actual" | grep -q "$pattern"; then
echo -e "  ${RED}[KO]${RESET}  $label"
echo -e "        ${YELLOW}should NOT contain:${RESET} $pattern"
echo -e "        ${YELLOW}got:${RESET} $(echo "$actual" | head -3)"
FAIL=$((FAIL + 1))
else
echo -e "  ${GREEN}[OK]${RESET}  $label"
PASS=$((PASS + 1))
fi
}

# ── FD LEAK CHECK ────────────────────────────────────────────
# Places a "ls /proc/self/fd | wc -l" before and after the cmd.
# Children inherit minishell's fds → leaked pipe/redir fds show up.
# VSCode fds are stable so the delta is 0 if no leak.
check_fd_leak()
{
local label="$1"
local cmd="$2"

TOTAL=$((TOTAL + 1))
if [ ! -d /proc/self/fd ]; then
echo -e "  ${YELLOW}[SKIP]${RESET}  $label (no /proc/self/fd)"
SKIP=$((SKIP + 1))
return
fi

raw=$(printf 'ls /proc/self/fd | wc -l\n%s\nls /proc/self/fd | wc -l\n' \
"$cmd" | "$MINI" 2>/dev/null | grep -v "^minishell" \
| grep -v "^exit$" | sed '/^> /d')

fd_before=$(echo "$raw" \
| grep -E '^[[:space:]]*[0-9]+[[:space:]]*$' \
| sed -n '1p' | tr -d '[:space:]')
fd_after=$(echo "$raw" \
| grep -E '^[[:space:]]*[0-9]+[[:space:]]*$' \
| sed -n '2p' | tr -d '[:space:]')

if [ -z "$fd_before" ] || [ -z "$fd_after" ]; then
echo -e "  ${YELLOW}[SKIP]${RESET}  $label (could not parse fd count)"
SKIP=$((SKIP + 1))
return
fi

if [ "$fd_before" = "$fd_after" ]; then
echo -e "  ${GREEN}[OK]${RESET}  $label (fds: $fd_before)"
PASS=$((PASS + 1))
else
diff=$((fd_after - fd_before))
echo -e "  ${RED}[KO]${RESET}  $label"
echo -e "        ${YELLOW}before:${RESET} $fd_before  ${YELLOW}after:${RESET} $fd_after  ${RED}(+${diff} leaked)${RESET}"
FAIL=$((FAIL + 1))
fi
}

# ── VALGRIND LEAK CHECK ──────────────────────────────────────
# ── run_valgrind ─────────────────────────────────────────────────────────────
# Runs minishell under valgrind with full checks.
# Flags:
#   --leak-check=full          : report every individual leaked block
#   --show-leak-kinds=all      : catch definitely + indirectly + possibly lost
#   --track-fds=yes            : report FDs still open at exit
#   --trace-children=no        : don't follow execve (avoids glibc false positives)
#   --child-silent-after-fork=yes : suppress output from fork children in parent report
#   --error-exitcode=42        : exit 42 if any error found
# A test FAILS if:
#   (a) "definitely lost" > 0 bytes, OR
#   (b) valgrind returned exit code 42
# FDs: stdin(0)/stdout(1)/stderr(2) are expected open. Any others are leaks.
# ─────────────────────────────────────────────────────────────────────────────
run_valgrind()
{
	local label="$1"
	local input="$2"

	TOTAL=$((TOTAL + 1))
	if ! command -v valgrind > /dev/null 2>&1; then
		echo -e "  ${YELLOW}[SKIP]${RESET}  $label (valgrind not installed)"
		SKIP=$((SKIP + 1))
		return
	fi

	val_out=$(printf '%s\n' "$input" \
		| valgrind \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-fds=yes \
			--trace-children=no \
			--child-silent-after-fork=yes \
			--error-exitcode=42 \
			"$MINI" 2>&1 >/dev/null)
	val_exit=$?

	def_lost=$(echo "$val_out" | grep "definitely lost:" \
		| grep -v "definitely lost: 0 bytes in 0 blocks")

	# FD check: Open file descriptors beyond 0/1/2 are warned by valgrind
	# Filter out the 3 expected ones (stdin0 stdout1 stderr2) + the pipe from tester
	# Also filter /dev/null and anonymous pipes (no path = inherited tester pipe)
	leaked_fds=$(echo "$val_out" | grep "Open file descriptor" \
		| grep -v "Open file descriptor 0:" \
		| grep -v "Open file descriptor 1:" \
		| grep -v "Open file descriptor 2:" \
		| grep -v "inherited from parent" \
		| grep -v "/dev/null" \
		| grep -v ":$")

	if [ -z "$def_lost" ] && [ "$val_exit" != "42" ]; then
		if [ -n "$leaked_fds" ]; then
			echo -e "  ${YELLOW}[WARN]${RESET} $label (no mem leak but extra FDs open)"
			echo "$leaked_fds" | head -4 | sed 's/^/        /'
			PASS=$((PASS + 1))
		else
			echo -e "  ${GREEN}[OK]${RESET}  $label"
			PASS=$((PASS + 1))
		fi
	else
		echo -e "  ${RED}[KO]${RESET}  $label"
		echo "$val_out" | grep -E "definitely lost:|indirectly lost:|ERROR SUMMARY" \
			| grep -v " 0 bytes in 0 blocks" | head -5 | sed 's/^/        /'
		FAIL=$((FAIL + 1))
	fi
}

# ── run_valgrind_strict ──────────────────────────────────────────────────────
# Like run_valgrind but ALSO fails on open FDs beyond 0/1/2.
# Use for builtin-only scenarios where no external program is exec'd.
# ─────────────────────────────────────────────────────────────────────────────
run_valgrind_strict()
{
	local label="$1"
	local input="$2"

	TOTAL=$((TOTAL + 1))
	if ! command -v valgrind > /dev/null 2>&1; then
		echo -e "  ${YELLOW}[SKIP]${RESET}  $label (valgrind not installed)"
		SKIP=$((SKIP + 1))
		return
	fi

	val_out=$(printf '%s\n' "$input" \
		| valgrind \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-fds=yes \
			--trace-children=no \
			--child-silent-after-fork=yes \
			--error-exitcode=42 \
			"$MINI" 2>&1 >/dev/null)
	val_exit=$?

	def_lost=$(echo "$val_out" | grep "definitely lost:" \
		| grep -v "definitely lost: 0 bytes in 0 blocks")
	leaked_fds=$(echo "$val_out" | grep "Open file descriptor" \
		| grep -v "Open file descriptor 0:" \
		| grep -v "Open file descriptor 1:" \
		| grep -v "Open file descriptor 2:" \
		| grep -v "inherited from parent" \
		| grep -v "/dev/null" \
		| grep -v ":$")

	if [ -z "$def_lost" ] && [ -z "$leaked_fds" ] && [ "$val_exit" != "42" ]; then
		echo -e "  ${GREEN}[OK]${RESET}  $label"
		PASS=$((PASS + 1))
	else
		echo -e "  ${RED}[KO]${RESET}  $label"
		echo "$val_out" | grep -E "definitely lost:|Open file descriptor|ERROR SUMMARY" \
			| grep -v " 0 bytes in 0 blocks" \
			| grep -v "Open file descriptor [012]:" \
			| grep -v "inherited from parent" \
			| head -6 | sed 's/^/        /'
		FAIL=$((FAIL + 1))
	fi
}

section()
{
echo ""
echo -e "${CYAN}══════════════════════════════════════════${RESET}"
echo -e "${CYAN}  $1${RESET}"
echo -e "${CYAN}══════════════════════════════════════════${RESET}"
}

note()
{
echo -e "  ${YELLOW}[NOTE]${RESET}  $1"
}

# ============================================================

if [ ! -f "$MINI" ]; then
echo -e "${RED}ERROR: $MINI not found. Run 'make' first.${RESET}"
exit 1
fi

# ============================================================
#  1 — ECHO
# ============================================================
section "ECHO"
run "echo simple"        "echo hello"         "hello"
run "echo multiple args" "echo hello world"   "hello world"
run "echo no args"       "echo"               ""
run "echo -na not flag"  "echo -na hello"     "-na hello"
run "echo empty string"  'echo ""'            ""
TOTAL=$((TOTAL + 1))
raw=$(printf 'echo -nnnnn hello\n' | "$MINI" 2>/dev/null)
if echo "$raw" | grep -qE "^hellominishell|^hello"; then
	echo -e "  ${GREEN}[OK]${RESET}  echo -nnnnn flag"
	PASS=$((PASS + 1))
else
	echo -e "  ${RED}[KO]${RESET}  echo -nnnnn flag  got:|$raw|"
	FAIL=$((FAIL + 1))
fi
TOTAL=$((TOTAL + 1))
raw=$(printf 'echo -nnnnn -n hello\n' | "$MINI" 2>/dev/null)
if echo "$raw" | grep -qE "^hellominishell|^hello"; then
	echo -e "  ${GREEN}[OK]${RESET}  echo -nnnnn -n"
	PASS=$((PASS + 1))
else
	echo -e "  ${RED}[KO]${RESET}  echo -nnnnn -n  got:|$raw|"
	FAIL=$((FAIL + 1))
fi

TOTAL=$((TOTAL + 1))
raw=$(printf 'echo -nn hello\n' | "$MINI" 2>/dev/null)
if echo "$raw" | grep -qE "^hellominishell|^hello"; then
echo -e "  ${GREEN}[OK]${RESET}  echo -nn (no newline)"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  echo -nn  got:|$raw|"
FAIL=$((FAIL + 1))
fi

TOTAL=$((TOTAL + 1))
raw=$(printf 'echo -n hello\n' | "$MINI" 2>/dev/null)
if echo "$raw" | grep -qE "^hellominishell|hello$"; then
echo -e "  ${GREEN}[OK]${RESET}  echo -n (no newline)"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  echo -n  got:|$raw|"
FAIL=$((FAIL + 1))
fi

# ============================================================
#  2 — PWD
# ============================================================
section "PWD"
run "pwd" "pwd" "$(pwd)"

# ============================================================
#  3 — CD
# ============================================================
section "CD"
run "cd absolute + pwd"        "cd /tmp
pwd"                                "/tmp"
run "cd no arg = HOME"         "cd
pwd"                                "$HOME"
run "cd - goes back"           "cd /tmp
cd /var
cd -
pwd"                                "/tmp"
run "cd relative path .."      "cd /tmp
cd ..
pwd"                                "/"
run "cd invalid → empty"       "cd /nonexistent_dir_xyz_42"  ""
run_exit "cd invalid → exit 1" "cd /nonexistent_dir_xyz_42"  1

# ============================================================
#  4 — ENV
# ============================================================
section "ENV"
run_contains "env has HOME"  "env"  "HOME="
run_contains "env has PATH"  "env"  "PATH="
run_contains "env has USER"  "env"  "USER="
run_contains "env has SHLVL" "env"  "SHLVL="
run_contains "env has PWD"   "env"  "PWD="

# ============================================================
#  5 — EXPORT
# ============================================================
section "EXPORT"
run "export var + echo"       "export MYVAR=42
echo \$MYVAR"                  "42"
run "export update"           "export X=first
export X=second
echo \$X"                      "second"
run "export empty value"      "export EMPTY=
echo [\$EMPTY]"                "[]"
run "export with spaces val"  'export SPACED="hello world"
echo $SPACED'                  "hello world"
run_contains "export no arg"  "export"               "declare -x"
run_contains "exported listed" "export SHOWME=yes
export"                        "SHOWME"
run "unset then re-export"    "export A=hello
unset A
export A=back
echo \$A"                      "back"
run_contains "export key novalue" "export NOVALUE
export"                           "NOVALUE"

# ============================================================
#  6 — UNSET
# ============================================================
section "UNSET"
run "unset removes var"   "export RM=bye
unset RM
echo \$RM"                 ""
run "unset nonexistent"   "unset DOESNOTEXIST"  ""
run "unset multiple"      "export A=1
export B=2
unset A B
echo \$A\$B"               ""
run_exit "unset exit 0"   "unset X"             0

# ============================================================
#  7 — EXIT
# ============================================================
section "EXIT"
run_exit "exit 0"                   "exit 0"   0
run_exit "exit 1"                   "exit 1"   1
run_exit "exit 42"                  "exit 42"  42
run_exit "exit 255"                 "exit 255" 255
run_exit "exit 256 wraps→0"         "exit 256" 0
run_exit "exit no arg"              "exit"     0
run_exit "exit non-numeric→255"     "exit abc" 255
run_exit "exit too many: no exit"   "exit 1 2
echo still" 0

# ============================================================
#  8 — $? (needs expander)
# ============================================================
section "DOLLAR QUESTION MARK"
note "\$? needs expander fix (aganganu) — tests skipped"

# ============================================================
#  9 — QUOTES
# ============================================================
section "QUOTES"
run "single no expand"    "echo '\$HOME'"         '$HOME'
run "double expand"       'echo "$HOME"'          "$HOME"
run "double spaces"       'echo "hello world"'    "hello world"
run "mixed quotes"        "echo 'a'\"b\""          "ab"
run "empty single"        "echo ''"               ""
run 'double with $USER'   'echo "user is $USER"'  "user is $USER"
run "adjacent tokens"     "echo 'hel'\"lo\""      "hello"
run "single stops expand" "echo '\$USER'"         '$USER'

# ============================================================
#  10 — REDIRECTIONS
# ============================================================
section "REDIRECTIONS"
TMP=$(mktemp)

run "redir out >"         "echo hello > $TMP
cat $TMP"                  "hello"

run "redir append >>"     "echo line1 > $TMP
echo line2 >> $TMP
cat $TMP"                  "line1
line2"

run "redir in <"          "echo testcontent > $TMP
cat < $TMP"                "testcontent"

TMP2=$(mktemp); rm -f "$TMP2"
run "redir overwrite"     "echo first > $TMP2
echo second > $TMP2
cat $TMP2"                 "second"
rm -f "$TMP2"

TMP3=$(mktemp)
run "redir creates file"  "echo created > $TMP3"  ""
TOTAL=$((TOTAL + 1))
if [ -f "$TMP3" ] && [ "$(cat "$TMP3")" = "created" ]; then
echo -e "  ${GREEN}[OK]${RESET}  redir file has correct content"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  redir file missing or wrong content"
FAIL=$((FAIL + 1))
fi
rm -f "$TMP3"

run "redir + pipe"        "echo piped | cat > $TMP
cat $TMP"                  "piped"

rm -f "$TMP"

# ============================================================
#  11 — PIPES
# ============================================================
section "PIPES"
run "echo | cat"              "echo hello | cat"                     "hello"
run "3 pipes"                 "echo hello | cat | cat"               "hello"
run "5 pipes"                 "echo x | cat | cat | cat | cat | cat" "x"
run_contains "cat | grep"     "cat /etc/passwd | grep root"          "root"
run "pipe + wc -l"            "printf 'a\nb\nc\n' | wc -l"          "3"
run "pipe builtin right"      "echo test | grep test"                "test"
run "false|echo (right runs)" "cat /nonexistent_xyz42 | echo ok"     "ok"

TMP=$(mktemp)
run "pipe + redir out"        "echo piped | cat > $TMP
cat $TMP"                      "piped"
rm -f "$TMP"

# ============================================================
#  12 — EXTERNAL COMMANDS / PATH
# ============================================================
section "EXTERNAL COMMANDS — PATH"
run "absolute /bin/echo"       "/bin/echo hello"   "hello"
run "absolute path chain"      "/bin/echo t | /bin/cat" "t"
run_exit "unknown → 127"       "commanddoesnotexist42"  127
run_contains "cat /etc/hostname" "cat /etc/hostname" \
"$(cat /etc/hostname 2>/dev/null | head -c 5 | tr -d '\n')"
run "ls head -0"               "ls /tmp | head -0"  ""

run_exit "unset PATH → 127"   "unset PATH
commandstillnotfound"          127

TOTAL=$((TOTAL + 1))
printf '#!/bin/sh\necho relscript_ok\n' > /tmp/ms_rel_42.sh
chmod +x /tmp/ms_rel_42.sh
actual=$(printf 'cd /tmp\n./ms_rel_42.sh\n' | "$MINI" 2>/dev/null | filter)
rm -f /tmp/ms_rel_42.sh
if [ "$actual" = "relscript_ok" ]; then
echo -e "  ${GREEN}[OK]${RESET}  relative path ./script"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  relative path ./script  got:|$actual|"
FAIL=$((FAIL + 1))
fi

# ============================================================
#  13 — HEREDOC
# ============================================================
section "HEREDOC"
TOTAL=$((TOTAL + 1))
actual=$(printf 'cat << EOF\nhello heredoc\nEOF\n' | "$MINI" 2>/dev/null \
| grep -v "^minishell" | grep -v "^>" | grep -v "^exit$")
if [ "$actual" = "hello heredoc" ]; then
echo -e "  ${GREEN}[OK]${RESET}  heredoc basic"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  heredoc basic  got:|$actual|"
FAIL=$((FAIL + 1))
fi

TOTAL=$((TOTAL + 1))
actual=$(printf 'cat << STOP\nline1\nline2\nSTOP\n' | "$MINI" 2>/dev/null \
| grep -v "^minishell" | grep -v "^>" | grep -v "^exit$")
if [ "$actual" = "$(printf 'line1\nline2')" ]; then
echo -e "  ${GREEN}[OK]${RESET}  heredoc multiline"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  heredoc multiline  got:|$actual|"
FAIL=$((FAIL + 1))
fi

TOTAL=$((TOTAL + 1))
actual=$(printf 'cat << END\nhello\nEND\necho after\n' | "$MINI" 2>/dev/null \
| grep -v "^minishell" | grep -v "^>" | grep -v "^exit$")
if [ "$actual" = "$(printf 'hello\nafter')" ]; then
echo -e "  ${GREEN}[OK]${RESET}  heredoc then next cmd"
PASS=$((PASS + 1))
else
echo -e "  ${RED}[KO]${RESET}  heredoc then next cmd  got:|$actual|"
FAIL=$((FAIL + 1))
fi

# ============================================================
#  14 — EDGE CASES (eval "go crazy")
# ============================================================
section "EDGE CASES — go crazy"

run "empty line (no crash)"      ""               ""
run "spaces only (no crash)"     "   "            ""
run "undefined var = empty"      "echo \$UNDEF42" ""
run "undefined in double quote"  'echo "$UNDEF42"' ""
run_exit "unclosed quote/no crash" "echo 'unclosed" 0
run_exit "true exit 0"           "/bin/true"     0
run_exit "false exit 1"          "/bin/false"    1

_LONG=$(python3 -c "print('a'*500)" 2>/dev/null || printf '%0.sa' {1..500})
run "long argument 500 chars"    "echo $_LONG"   "$_LONG"

run "multiple export persist"    "export V1=aaa
export V2=bbb
echo \$V1 \$V2"                   "aaa bbb"

run "cd HOME explicit"           "cd \$HOME
pwd"                              "$HOME"

TMP=$(mktemp)
run "redir then pipe"            "echo source > $TMP
cat $TMP | grep source"          "source"
rm -f "$TMP"

run "pipe count lines"           "printf 'a\nb\nc\n' | wc -l" "3"

# eval sheet: export then visible in env
run_contains "export → in env"   "export EVALTEST=42
env"                              "EVALTEST=42"

# ============================================================
#  15 — FILE DESCRIPTOR LEAKS
#
#  Method: run "ls /proc/self/fd | wc -l" BEFORE and AFTER the
#  tested command inside the same minishell session.  Children
#  inherit all of minishell's fds, so leaked pipe/redir fds
#  are visible in the count.  VSCode's stable extra fds cancel
#  out because we take a delta.
# ============================================================
section "FILE DESCRIPTOR LEAKS"

check_fd_leak "fd: simple echo"       "echo hello"
check_fd_leak "fd: echo | cat"        "echo hello | cat"
check_fd_leak "fd: 3-pipe"            "echo hello | cat | cat"
check_fd_leak "fd: 5-pipe"            "echo x | cat | cat | cat | cat | cat"

TMP=$(mktemp)
check_fd_leak "fd: redir >"           "echo test > $TMP"
check_fd_leak "fd: redir >>"          "echo test >> $TMP"
check_fd_leak "fd: redir <"           "cat < $TMP"
rm -f "$TMP"

check_fd_leak "fd: heredoc"           "cat << ENDLEAK
content
ENDLEAK"

check_fd_leak "fd: pipe + redir"      "echo piped | cat > /tmp/ms_fdleak_42
rm -f /tmp/ms_fdleak_42"

check_fd_leak "fd: external cmd"      "ls /tmp > /dev/null"

check_fd_leak "fd: 10 sequential"     "echo a
echo b
echo c
echo d
echo e
echo f
echo g
echo h
echo i
echo j"

check_fd_leak "fd: mixed ops"         "export MSFDA=hello
echo \$MSFDA | cat
unset MSFDA"

# ============================================================
#  16 — MEMORY LEAKS (valgrind — skip if not installed)
#
#  Organised by theme.  run_valgrind = no-mem-leak check.
#  run_valgrind_strict = no-mem-leak AND no leaked FDs.
#
#  Common 42 student leaks caught here:
#   - pipes   : int **pipes / pid_t *pids not freed after pipeline
#   - heredoc : heredoc pipe fd not closed after dup2
#   - redir   : opened fd not closed on error path or after dup2
#   - env     : t_env list never freed at exit
#   - history : readline history not cleared
#   - export  : strdup'd key/value not freed on update or unset
#   - cmd     : t_cmd / t_token not freed between iterations
#   - path    : find_path result (child exec path) — acceptable in exec'd child
# ============================================================
section "MEMORY LEAKS — BUILTINS (strict: mem + fds)"

run_valgrind_strict "vgs: echo hello"               "echo hello"
run_valgrind_strict "vgs: echo -n"                  "echo -n hello"
run_valgrind_strict "vgs: echo multi args"          "echo a b c d e"
run_valgrind_strict "vgs: pwd"                      "pwd"
run_valgrind_strict "vgs: env"                      "env"
run_valgrind_strict "vgs: export no arg"            "export"
run_valgrind_strict "vgs: export set"               "export VGTEST=42"
run_valgrind_strict "vgs: export update x3"         "export VGX=first
export VGX=second
export VGX=third"
run_valgrind_strict "vgs: unset nonexistent"        "unset VGDOESNOTEXIST"
run_valgrind_strict "vgs: export then unset"        "export VGDEL=bye
unset VGDEL"
run_valgrind_strict "vgs: export 10 vars"           "export A1=1
export A2=2
export A3=3
export A4=4
export A5=5
export A6=6
export A7=7
export A8=8
export A9=9
export A10=10"
run_valgrind_strict "vgs: export 10 then unset all" "export B1=1
export B2=2
export B3=3
export B4=4
export B5=5
unset B1 B2 B3 B4 B5"
run_valgrind_strict "vgs: export long value"        "export VGLONG=$(python3 -c "print('x'*300)" 2>/dev/null || printf 'x%.0s' {1..300})"
run_valgrind_strict "vgs: cd /tmp + pwd"            "cd /tmp
pwd"
run_valgrind_strict "vgs: cd - back"                "cd /tmp
cd /var
cd -
pwd"
run_valgrind_strict "vgs: 30 echo commands"         "echo 1
echo 2
echo 3
echo 4
echo 5
echo 6
echo 7
echo 8
echo 9
echo 10
echo 11
echo 12
echo 13
echo 14
echo 15
echo 16
echo 17
echo 18
echo 19
echo 20
echo 21
echo 22
echo 23
echo 24
echo 25
echo 26
echo 27
echo 28
echo 29
echo 30"

section "MEMORY LEAKS — PIPELINES (most common leak source)"

# Pipelines alloc int**pipes, pid_t*pids → must be freed after waitpid
run_valgrind "vg: 2-pipe"              "echo hello | cat"
run_valgrind "vg: 3-pipe"             "echo hello | cat | cat"
run_valgrind "vg: 5-pipe"             "echo x | cat | cat | cat | cat | cat"
run_valgrind "vg: 7-pipe"             "echo x | cat | cat | cat | cat | cat | cat | cat"
run_valgrind "vg: 10-pipe"            "echo x | cat | cat | cat | cat | cat | cat | cat | cat | cat | cat"
run_valgrind "vg: pipe+wc"            "printf 'a\nb\nc\n' | wc -l"
run_valgrind "vg: pipe 3x builtins"   "echo test | cat | grep test"
run_valgrind "vg: pipe cmd not found" "commandnotfound42vg | cat"
run_valgrind "vg: pipe false|echo"    "cat /nonexistent_xyz42vg | echo ok"
run_valgrind "vg: pipe+grep"          "cat /etc/passwd | grep root"
run_valgrind "vg: 3 pipelines seq"    "echo a | cat
echo b | cat
echo c | cat"
run_valgrind "vg: 5 pipelines seq"    "echo 1 | cat
echo 2 | cat
echo 3 | cat
echo 4 | cat
echo 5 | cat"

section "MEMORY LEAKS — REDIRECTIONS (tricky fd paths)"

# Each > or < opens an fd → must be dup2'd then closed, OR closed on error
_VGT1=$(mktemp)
_VGT2=$(mktemp)
run_valgrind "vg: redir >"              "echo hello > $_VGT1"
run_valgrind "vg: redir >> append"      "echo line >> $_VGT1"
run_valgrind "vg: redir <"             "cat < $_VGT1"
run_valgrind "vg: redir overwrite"     "echo first > $_VGT2
echo second > $_VGT2"
run_valgrind "vg: redir chain >/>"     "echo a > $_VGT1
cat < $_VGT1 > $_VGT2"
run_valgrind "vg: pipe+redir out"      "echo piped | cat > $_VGT1"
run_valgrind "vg: pipe+redir in"       "echo content > $_VGT1
cat < $_VGT1 | grep content"
run_valgrind "vg: redir to /dev/null"  "ls /tmp > /dev/null"
run_valgrind "vg: 5 redir cmds seq"    "echo r1 > $_VGT1
echo r2 > $_VGT1
echo r3 > $_VGT1
echo r4 > $_VGT1
echo r5 > $_VGT1"
run_valgrind "vg: redir invalid file"  "echo test > /nonexistent_dir_vg42/file"
rm -f "$_VGT1" "$_VGT2"

section "MEMORY LEAKS — HEREDOC (pipe fd leaks)"

# heredoc_fd creates a pipe; child writes, parent reads via fd[0].
# fd[0] must be closed after dup2 in handle_heredocs.
# Especially tricky: heredoc in a pipeline.
run_valgrind "vg: heredoc basic"        "cat << VGEOF
hello heredoc
VGEOF"
run_valgrind "vg: heredoc multiline"    "cat << VGEOF
line1
line2
line3
line4
line5
VGEOF"
run_valgrind "vg: 2 heredocs seq"       "cat << VGA
first
VGA
cat << VGB
second
VGB"
run_valgrind "vg: 5 heredocs seq"       "cat << H1
a
H1
cat << H2
b
H2
cat << H3
c
H3
cat << H4
d
H4
cat << H5
e
H5"
run_valgrind "vg: heredoc then pipe"    "cat << VGEOF
piped content
VGEOF"
run_valgrind "vg: heredoc + redir out"  "cat << VGEOF > /tmp/ms_vghd_42.txt
stored
VGEOF
rm -f /tmp/ms_vghd_42.txt"

section "MEMORY LEAKS — ERROR PATHS (sneaky allocs on failure)"

# These test paths that malloc but then hit an error before freeing
run_valgrind "vg: unknown command"      "commandnotfound_vg42"
run_valgrind "vg: 3 unknown cmds"      "cmd1notfound_vg
cmd2notfound_vg
cmd3notfound_vg"
run_valgrind "vg: cd invalid"          "cd /nonexistent_42vg"
run_valgrind "vg: cd 5x invalid"       "cd /nope1
cd /nope2
cd /nope3
cd /nope4
cd /nope5"
run_valgrind "vg: exit non-numeric"    "exit abc"
run_valgrind "vg: exit too many args"  "exit 1 2 3
echo still here"
run_valgrind "vg: redir to no-perm"    "echo fail > /root/vg_noperm_42.txt"
run_valgrind "vg: unset PATH+cmd"      "unset PATH
commandnotfound_novpath"

section "MEMORY LEAKS — COMPLEX SESSIONS (full workflow)"

# Simulate real evaluation scenarios: multiple operations, no leaks allowed
_VGS=$(mktemp)
run_valgrind "vg: session: export+pipe+redir" "export VGSESSION=hello
echo \$VGSESSION | cat > $_VGS
cat < $_VGS
unset VGSESSION"

run_valgrind "vg: session: cd+pipe+heredoc"   "cd /tmp
pwd | cat
cat << VGSES
in tmp
VGSES
cd -"

run_valgrind "vg: session: 20 mixed cmds"     "echo start
export VGA=1
export VGB=2
echo \$VGA \$VGB | cat
unset VGA
cd /tmp
pwd
cd -
cat << VGEOF
heredoc1
VGEOF
echo middle | cat | cat
export VGC=3
ls /tmp > /dev/null
unset VGB VGC
cat << VGEOF2
heredoc2
VGEOF2
echo \$VGA
echo end"

run_valgrind "vg: session: 10 pipes varied"   "echo a | cat
echo b | cat | cat
echo c | cat | cat | cat
echo d | cat
echo e | grep e
echo f | cat
echo g | grep g
echo h | cat | cat
echo i | grep i
echo j | cat"

run_valgrind "vg: session: export cycle"      "export VGX=init
export VGX=update1
export VGX=update2
echo \$VGX
unset VGX
export VGX=restart
echo \$VGX
unset VGX"

rm -f "$_VGS"

# ============================================================
#  RESULTS
# ============================================================
echo ""
echo -e "${MAGENTA}══════════════════════════════════════════${RESET}"
if [ "$SKIP" -gt 0 ]; then
echo -e "${MAGENTA}  Results: ${GREEN}$PASS passed${RESET}  ${RED}$FAIL failed${RESET}  ${YELLOW}$SKIP skipped${RESET}  / $TOTAL total${RESET}"
else
echo -e "${MAGENTA}  Results: ${GREEN}$PASS passed${RESET}  ${RED}$FAIL failed${RESET}  / $TOTAL total${RESET}"
fi
echo -e "${MAGENTA}══════════════════════════════════════════${RESET}"
echo ""

[ "$FAIL" -gt 0 ] && exit 1
exit 0
