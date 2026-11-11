#!/usr/bin/env sh
# Generated with OpenAI Codex. Uses only the current user's existing X11 authorization.
set -eu
cd "$(dirname "$0")/.."
if ./build/heliograph --check-display >/dev/null 2>&1; then
 exec ./build/heliograph "$@"
fi
for auth in "$HOME/.Xauthority" /run/user/"$(id -u)"/.mutter-Xwaylandauth.* /run/user/"$(id -u)"/xauth_*; do
 if [ -r "$auth" ] && XAUTHORITY="$auth" ./build/heliograph --check-display >/dev/null 2>&1; then
  export XAUTHORITY="$auth"
  exec ./build/heliograph "$@"
 fi
done
printf '%s\n' 'No hay acceso autorizado a X11. Abre una terminal de tu escritorio sin sudo.' 'Ejecuta: echo "$DISPLAY"; echo "$XAUTHORITY"' 'No uses xhost +. Para probar sin ventana: make headless.' >&2
exit 1
