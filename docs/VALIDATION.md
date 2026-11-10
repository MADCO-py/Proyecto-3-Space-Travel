<!-- Generado con asistencia de OpenAI Codex. -->
# Verificación local · 10 de noviembre de 2026

## Entorno

Linux x86-64, compilador C del sistema, Raylib 5.5 compilado desde el tag oficial, ventana de 1440×900, OpenGL 4.5 core de Mesa 26.0.8. La pantalla virtual Xvfb usa **llvmpipe / LLVM 21.1.8**, rasterización de OpenGL en CPU; esto no convierte al código del proyecto en un software renderer propio. Límite de 60 FPS y MSAA solicitado de 4 muestras.

## Resultados

- `make all test`: compilación con `-Wall -Wextra -Wpedantic`, sin advertencias del código del proyecto. Pasaron barridos continuos, colisiones con cuerpos en movimiento, radios orbitales, 25 rutas entre destinos, tres niveles de LOD y casos OBJ válidos e inválidos.
- ASan + UBSan: pasaron las mismas pruebas sin errores detectados. Se desactivó LeakSanitizer porque este entorno instrumentado no permite su funcionamiento; **no se afirma haber verificado fugas con LSan**.
- `visual_check.sh`: se abrieron y capturaron los cinco planetas, sin `WARNING` / `ERROR` de Raylib. Se inspeccionaron las cinco capturas y el anillo, la nave, la iluminación y las estrellas.
- Salto completo: 240 cuadros, destino Pelagos, `SMOKE PASS`. La verificación confirma que terminó el viaje y que la cámara quedó junto al punto de llegada.
- Blender 5.2.2 LTS: guardó `assets/manta.blend`; el OBJ producido contiene 58 vértices y 41 caras originales (35 quads y 2 n-gons). La aplicación triangula ese archivo a 92 triángulos.

### Rendimiento medido

Los cuadros incluyen la espera del límite de 60 FPS. Se excluyen los primeros 30 cuadros de las cifras de calentamiento y la exportación final de PNG. Son muestras locales, no una garantía para otros equipos.

| Escenario | Muestra tras calentamiento | Media | p95 | Máximo |
|---|---:|---:|---:|---:|
| Tránsito completo | 210 cuadros | 16,68 ms | 16,71 ms | 17,07 ms |
| Muy cerca de Nival, LOD alto, separación cámara/superficie de 4,50 unidades | 210 cuadros | 18,42 ms | 19,64 ms | 26,97 ms |

Las capturas breves de cada planeta (40 cuadros, últimos 10 calentados) dieron medias de 16,68 a 17,30 ms. La primera compilación JIT de shaders en el driver llegó a tardar entre 0,6 y 0,85 segundos en pruebas iniciales; no está incluida en las cifras estables.

Reproducir el estrés de cercanía:

```sh
xvfb-run -a -s '-screen 0 1440x900x24' ./build/heliograph --surface --body 3 --frames 240 --capture build/surface.png
```

`--surface` pausa las órbitas para sostener una posición de estrés reproducible. El programa normal conserva las órbitas activas. La verificación automatizada no sustituye una sesión manual de todos los controles en el escritorio del usuario. El CI remoto no se ha ejecutado durante la preparación local.
