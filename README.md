Uruchamianie:
e.g.
mpirun -n 6 blockage 100 1000 4 images/

Generacja filmu:
ffmpeg -framerate 3 -i images/%d.pgm -vf "scale=iw*10:ih*10" -c:v libx264 -pix_fmt yuv420p output.mp4

TODOS:
- OpenMP
- Badania
- operacje kolektywne (+10)
