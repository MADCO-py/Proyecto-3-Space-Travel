<!-- Generated with OpenAI Codex. -->
# Verificación del renderer CPU

10 de noviembre de 2026. Se sustituyó el renderer GPU anterior.

- Compilación C99 con `-Wall -Wextra -Wpedantic`, sin advertencias.
- `make test`: pasan invariantes orbitales, colisiones, LOD, 25 rutas de viaje y triangulación OBJ.
- Prueba sin ventana: 10 cuadros cercanos a Pelagos a 800×500; media de 26,11 ms por cuadro en este entorno. 5335 triángulos rasterizados y 15407 caras descartadas en el último cuadro. No es una garantía de rendimiento en otros equipos.
- Con `DISPLAY=:998` inexistente, el programa devuelve código 1 y un mensaje sobre X11; no hay violación de segmento.
- Captura CPU inspeccionada: planeta iluminado, texturas, cielo, nave y HUD visibles.

La autorización de la sesión real del usuario no puede darse por corregida desde una pantalla virtual. `tools/run.sh` prueba credenciales X11 existentes del mismo usuario. No se han reutilizado las mediciones GPU anteriores ni se afirma verificada una nota académica.
