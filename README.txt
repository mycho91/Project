이 소스코드는 GitHup에 올라온 다른사람들의 라이브러리들을 활용하여 만들었습니다.
서버에 아령 한개만 연결해도 동작을 하지만, 아령이 2개라는게 전제되어있습니다.
따라서 아령을 두개를 만드시거나, 동봉된 FakeDumbell을 이용해 아령이 두개일때의 동작을
확인하시면 됩니다.

컴파일 방법
SmartDumbell : gcc main.c pwm.c dma.c mailbox.c ws2811.c board_info.c -lwiringPi -lm
other : gcc <filename>

