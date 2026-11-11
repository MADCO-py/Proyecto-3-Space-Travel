<!-- Elaborado con asistencia de OpenAI Codex. -->
# HELIOGRAPH

# [Aquí está el video del proyecto](https://youtu.be/EQ4iENWJLYk)

Simulación de un sistema solar con un sol, cuatro planetas y Oculus, un ojo humano que también orbita la estrella. Incluye una nave que sigue a la cámara.

La escena se renderiza en CPU: triángulos, texturas, iluminación y z-buffer. Raylib se utiliza para ventana, entrada y matemáticas; OpenGL solo presenta el framebuffer terminado.

## Ejecutar

Requiere Linux, un compilador C, Make, curl y las bibliotecas de desarrollo de X11 y OpenGL. En Ubuntu/Debian:

```sh
sudo apt-get install build-essential curl libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
```

Desde la carpeta del proyecto:

```sh
make setup
make run
```

Las dependencias se descargan en `zig-pkg/`, que está excluido de Git. Si aparece un error de autorización X11, ejecuta desde una terminal de tu escritorio, sin `sudo`.

## Controles

- **WASD:** mover la cámara.
- **Espacio / Ctrl:** subir y bajar.
- **Shift:** acelerar.
- **Clic derecho mantenido o flechas:** mirar.
- **1–5 y Enter:** seleccionar destino e iniciar el viaje.
- **P:** pausar órbitas. **O:** mostrar u ocultar trayectorias.
- **H:** ayuda. **Esc:** salir.

## Características

Texturas procedurales y efectos distintos por cuerpo, iluminación desde el sol, back-face culling, tres niveles de detalle, cielo estrellado, colisiones, movimiento 3D y viaje rápido animado. El cargador OBJ admite quads y polígonos cóncavos simples.

Las texturas se generan una vez al iniciar, combinando colores y patrones matemáticos. Oculus tiene esclerótica, iris marrón, pupila animada y reflejo corneal.

## Pruebas

```sh
make test
make headless
```

## Atribución

El código, las texturas procedurales y la nave se desarrollaron con asistencia de **OpenAI Codex**. El archivo `assets/manta.blend` está incluido, pero no acredita modelado personal del alumno. Más detalles en [ATTRIBUTION.md](ATTRIBUTION.md).
