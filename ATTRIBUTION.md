# Procedencia y atribución

El código de `src/`, `tests/`, los scripts de `tools/`, el Makefile, el CI y la documentación se desarrollaron con asistencia de **OpenAI Codex**, por solicitud del usuario. Los archivos de código incluyen comentarios de atribución.

Las texturas se generan en CPU en `make_textures`, dentro de `src/main.c`. Cada textura es una imagen de 512 × 256 píxeles. Se calculan patrones con seno y coseno, se combinan paletas distintas y se guarda el resultado en el arreglo `textures`. No se descargaron imágenes para las superficies. `sample` consulta estos colores mediante coordenadas UV y `shade` aplica iluminación y efectos animados. El cielo y el HUD también se calculan en CPU.

La geometría de **MANTA-07**, `assets/manta.obj` y `assets/manta.blend`, fue generada mediante `tools/model_ship.py` con asistencia de Codex. El archivo Blender contiene atribución y no acredita modelado personal del alumno.

## Dependencias

- **Raylib 5.5**, Ramon Santamaria y colaboradores, licencia zlib/libpng: https://github.com/raysan5/raylib/tree/5.5. Su código y licencia se mantienen en `zig-pkg/raylib-5.5/`, excluido de Git. La versión activa utiliza su ventana, entrada, tiempo y matemáticas; no sus funciones de dibujo, generación de mallas ni rasterización de la escena.
- **OpenGL** presenta exclusivamente el framebuffer terminado por la CPU. No calcula los planetas, la nave, su iluminación ni el z-buffer de la escena.
- **Blender** se usó para guardar el archivo `.blend` desde el script atribuido. No hace falta para ejecutar la simulación.

Se consultaron la cabecera y el ejemplo de iluminación oficiales de Raylib durante el desarrollo inicial. El ejemplo no fue copiado al proyecto. La versión GPU inicial fue sustituida; sus shaders, fuente externa y capturas obsoletas fueron retirados.

La procedencia de la asistencia se mantiene visible. La aceptación académica y los puntos dependen del docente.

Oculus reemplaza únicamente al quinto planeta por un ojo humano procedural: esclerótica marfil, venas discretas, iris marrón con fibras radiales, pupila negra de dilatación suave y brillo corneal. Conserva órbita, colisión, LOD y rasterización CPU. Su textura y shader fueron desarrollados con asistencia de Codex.
