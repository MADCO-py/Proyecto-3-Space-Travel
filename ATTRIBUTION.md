# Procedencia y atribución

El código de `src/`, `tests/`, los shaders, los scripts de `tools/`, el Makefile, la configuración de CI y la documentación fueron generados con asistencia de **OpenAI Codex**, por solicitud y con autorización del usuario, el 10 de noviembre de 2026. Los archivos de código incluyen comentarios de atribución. No se presentan como trabajo escrito íntegramente por el alumno.

El diseño y la geometría de **MANTA-07**, tanto `assets/manta.obj` como `assets/manta.blend`, se generaron mediante `tools/model_ship.py` con asistencia de Codex. El archivo Blender contiene una propiedad de atribución. **Su existencia no demuestra que el estudiante haya modelado personalmente la nave y no debe usarse para reclamar ese bono.**

Las texturas de planetas y anillos, la textura de halo y el cielo estrellado se generan proceduralmente por código atribuido a Codex; no contienen imágenes descargadas.

## Dependencias y referencias

- **Raylib 5.5**, de Ramon Santamaria y colaboradores, licencia zlib/libpng. Descarga oficial: https://github.com/raysan5/raylib/tree/5.5. Su fuente y licencia se conservan en `zig-pkg/raylib-5.5/`, excluido de Git. Se utiliza su API, matemáticas, generación de esferas y rasterización OpenGL. El cargador OBJ de la nave es el incluido en `src/obj.c`, no el de Raylib.
- Se consultó la [cabecera oficial de Raylib 5.5](https://github.com/raysan5/raylib/blob/5.5/src/raylib.h) y el [ejemplo oficial de iluminación](https://github.com/raysan5/raylib/blob/5.5/examples/shaders/shaders_basic_lighting.c) para comprobar convenciones de API, atributos y uniforms. El ejemplo no fue copiado al proyecto.
- **DejaVu Sans**, fuente de interfaz distribuida con su licencia completa en `assets/fonts/LICENSE.txt`. No es una fuente creada por el alumno ni por Codex.
- **Blender** se utilizó para construir el archivo `.blend` mediante el script atribuido. No es una dependencia necesaria para ejecutar la simulación.

La consigna advierte que el trabajo generado por LLM puede ser sancionado. La autorización del usuario no garantiza que el docente lo acepte. Esta entrega mantiene su procedencia visible.
