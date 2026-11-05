#!/usr/bin/env bash
#
# Generate the reference values for testUF26Field with RUQI's magField.
#
# usage: Test/makeReferenceValues.sh <magField> <expDisk config> <ringDisk config>
#
# The configs must hold the parameters of Table 2 of arXiv:2608.21293, with
# the vertical scales of RUQI's HeightFunction given to full precision:
#   expDisk : z0Halo = 1/arcosh(e)   = 0.6033348 kpc (sech)
#   ringDisk: z0Halo = 0.9/arcosh(e) = 0.5430013 kpc (sech)
#             h0     = 1.9/sqrt(2)   = 1.3435029 kpc (gauss)
#
# Writes Test/referenceValues.txt (or the file given by $OUT).
#

set -euo pipefail

if [ $# -ne 3 ]; then
  sed -n '3,14p' "$0" | sed 's/^# \{0,1\}//'
  exit 1
fi

magField=$1
expDiskConfig=$2
ringDiskConfig=$3
models=(expDisk ringDisk)
out=${OUT:-$(dirname "$0")/referenceValues.txt}

config() {
  case $1 in
    expDisk) echo "$expDiskConfig" ;;
    ringDisk) echo "$ringDiskConfig" ;;
  esac
}

# galactocentric x y z in kpc (Sun at x = -8.178 kpc)
positions=(
  "0 0 0"          # Galactic center
  "0.5 0 0"        # cusp smoothing (expDisk), inner phase-out (ringDisk)
  "-1 0.5 0.2"
  "1 1 1"
  "-3 0 0"
  "3 -2 -0.5"
  "-4 0 0"         # just inside the annulus peak
  "-4.5 0 0"       # annulus peak
  "-5 0 0.3"       # just outside the annulus peak
  "0 4.5 1.5"
  "4.5 0 -1.5"     # mirror of the previous point (N/S symmetry)
  "-6 0 0"
  "-8.178 0 0"     # Sun
  "-8.178 0 0.5"
  "-8.178 0 -1"
  "-8.178 0 3"
  "0 -10 0.2"
  "7 7 -0.7"
  "-12 3 0"
  "-15 0 0"
  "-18 0 0"        # outer cutoff radius
  "16 10 0.1"
  "-25 0 0"
  "0 0 5"
  "-5 5 -8"
  "30 -30 1"
)

# |b| from magField's output line in microgauss
evaluate() {
  "$magField" --config="$1" --xPos="$2" --yPos="$3" --zPos="$4" \
    | awk '/x\/y\/z\//{print $NF}' | tail -n 1
}

# compare value $1 to expectation $2 (relative tolerance 1e-4)
close() {
  awk -v v="$1" -v e="$2" 'BEGIN{d = v/e - 1; exit !(d < 1e-4 && d > -1e-4)}'
}

# Sanity check at the Sun: b = b_d * S_d(R_sun), independent of the vertical
# profiles. The random models return b^2 per model (direction dummy), so
# depending on how magField combines them, it prints b or b^2.
sunExpected=4.367830407
sun=$(evaluate "$(config expDisk)" -8.178 0 0)
if [ -z "$sun" ]; then
  echo "could not read field from magField output" >&2
  exit 2
elif close "$sun" "$sunExpected"; then
  squared=0
elif close "$sun" "$(awk -v e=$sunExpected 'BEGIN{print e*e}')"; then
  squared=1
  echo "note: magField prints b^2 for random models, taking the square root"
else
  echo "expDisk at the Sun: magField gives $sun, expected $sunExpected microgauss" >&2
  echo "(zero means magField ignores randomAverage models)" >&2
  exit 3
fi

{
  echo "# UF26 reference values: rms random field b in microgauss"
  echo "# generated with RUQI magField on $(date -u +%Y-%m-%d)"
  for m in "${models[@]}"; do
    printf "#   %-9s %s\n" "$m:" "$(basename "$(config "$m")")"
  done
  printf "# %-9s %9s %9s %9s  %s\n" model x/kpc y/kpc z/kpc b/microgauss
  for m in "${models[@]}"; do
    for p in "${positions[@]}"; do
      read -r x y z <<< "$p"
      v=$(evaluate "$(config "$m")" "$x" "$y" "$z")
      awk -v m="$m" -v x="$x" -v y="$y" -v z="$z" -v v="$v" -v s="$squared" \
        'BEGIN{ if (s) v = sqrt(v); printf "%-11s %9.3f %9.3f %9.3f  %.5e\n", m, x, y, z, v }'
    done
  done
} > "$out"

# second sanity check, ringDisk at the Sun
ringSun=$(awk '$1 == "ringDisk" && $2 == -8.178 && $3 == 0 && $4 == 0 {print $5}' "$out")
if ! close "$ringSun" 4.139605409; then
  echo "warning: ringDisk at the Sun is $ringSun, expected 4.139605409 microgauss" >&2
fi

echo "wrote $(grep -vc '^#' "$out") reference values to $out"
