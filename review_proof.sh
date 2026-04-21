#!/usr/bin/env bash
# Script de verification pour la man review.
# Objectif: comparer ./hsh avec /bin/sh sur des cas autorises par le projet.

set -u

SHELL_BIN="./hsh"
REF_BIN="/bin/sh"

# Couleurs simples (lisibles meme sans support complet ANSI)
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
NC="\033[0m"

PASS=0
FAIL=0
INDEX=1

TESTS=(
  "/bin/echo OK"
  "ls"
  "ls -l /tmp"
  "env"
  "commande_introuvable_xyz"
)

run_case() {
  local cmd="$1"
  local out_hsh err_hsh out_ref err_ref
  local code_hsh code_ref
  local err_ref_norm
  local norm_hsh norm_ref

  out_hsh="$(echo "$cmd" | "$SHELL_BIN" 2> /tmp/hsh_err.$$)"
  code_hsh=$?
  err_hsh="$(cat /tmp/hsh_err.$$)"

  out_ref="$(echo "$cmd" | "$REF_BIN" 2> /tmp/ref_err.$$)"
  code_ref=$?
  err_ref="$(cat /tmp/ref_err.$$)"

  # Le sujet autorise une difference sur argv[0] dans les erreurs.
  err_ref_norm="${err_ref//$REF_BIN/$SHELL_BIN}"

  if [[ "$cmd" == "env" ]]; then
    norm_hsh="$(printf "%s\n" "$out_hsh" | grep -v '^_=' | sort)"
    norm_ref="$(printf "%s\n" "$out_ref" | grep -v '^_=' | sort)"
    out_hsh="$norm_hsh"
    out_ref="$norm_ref"
  fi

  if [[ "$out_hsh" == "$out_ref" && "$err_hsh" == "$err_ref_norm" && "$code_hsh" -eq "$code_ref" ]]; then
    printf "%b[%d] PASS%b %s\n" "$GREEN" "$INDEX" "$NC" "$cmd"
    PASS=$((PASS + 1))
  else
    printf "%b[%d] FAIL%b %s\n" "$RED" "$INDEX" "$NC" "$cmd"
    printf "%b  hsh stdout:%b %s\n" "$YELLOW" "$NC" "$out_hsh"
    printf "%b  ref stdout:%b %s\n" "$YELLOW" "$NC" "$out_ref"
    printf "%b  hsh stderr:%b %s\n" "$YELLOW" "$NC" "$err_hsh"
    printf "%b  ref stderr:%b %s\n" "$YELLOW" "$NC" "$err_ref_norm"
    printf "%b  hsh code:%b %d\n" "$YELLOW" "$NC" "$code_hsh"
    printf "%b  ref code:%b %d\n" "$YELLOW" "$NC" "$code_ref"
    FAIL=$((FAIL + 1))
  fi

  INDEX=$((INDEX + 1))
}

main() {
  local t

  if [[ ! -x "$SHELL_BIN" ]]; then
    echo "Erreur: $SHELL_BIN introuvable. Compile d'abord avec:"
    echo "gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh"
    exit 1
  fi

  echo "Verification de conformite: $SHELL_BIN vs $REF_BIN"
  echo "Cas testes: commandes simples, arguments, PATH, env, not found"
  echo

  for t in "${TESTS[@]}"; do
    run_case "$t"
  done

  rm -f /tmp/hsh_err.$$ /tmp/ref_err.$$

  echo
  printf "Resultat final: %bPASS=%d%b | %bFAIL=%d%b\n" "$GREEN" "$PASS" "$NC" "$RED" "$FAIL" "$NC"

  if [[ "$FAIL" -eq 0 ]]; then
    exit 0
  fi
  exit 1
}

main
