# Использование

__Входной формат данных__: 1 канал, S16_LE, 8000 Hz

Опции:
```
Allowed options:
  -h [ --help ]                 Print help
  -m [ --msec ] arg (=20)       Frame length in msec
  -c [ --channel ] arg (=1)     Channels to filter
  -r [ --rate ] arg (=8000)     Sample rate
  -f [ --filter ] arg (=13)     Gaussian filter size
  -p [ --peak ] arg (=0.950000) Max peak value
  -g [ --gain ] arg (=100)      Max gain value
  -v [ --verbose ]              Enable verbose logs
```

Обработка файла:
```
cat input.raw | ./alsa_agc > output.raw
```

Проигрывание файла:
```
cat input.raw | ./alsa_agc | aplay -D <output device> -c 1 -f S16_LE -t raw
```

Обработка в реальном времени:
```
arecord -D <input device> -f S16_LE -t raw | ./alsa_agc | aplay -D <output device> -f S16_LE -t raw
```
