@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\test_jnag\labels.tmp" -fI -W+ie -C V2E -o "C:\test_jnag\test_jnag.hex" -d "C:\test_jnag\test_jnag.obj" -e "C:\test_jnag\test_jnag.eep" -m "C:\test_jnag\test_jnag.map" "C:\test_jnag\main.asm"
