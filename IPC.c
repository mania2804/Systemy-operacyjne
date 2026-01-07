sender #include #include #include #include #include #include
const char *morse_codes[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
"-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
"..-", "...-", ".--", "-..-", "-.--", "--.."};

void send_pulse(pid_t target_pid, int duration)
{
kill(target_pid, SIGUSR1);
sleep(duration);
}

int main(int argc, char *argv[])
{
if (argc != 2)
{
printf("Uzycie: %s \n", argv[0]);
return 1;
}

pid_t target_pid = atoi(argv[1]);
char input[100];

printf("Wpisz tekst od A do Z i nacisnij ENTER: ");
printf("Połączenie \n");
kill(target_pid, SIGUSR1);
sleep(1);

while (fgets(input, sizeof(input), stdin))
{
for (int i = 0; input[i] != '\0'; i++)
{
char c = toupper(input[i]);

if (c >= 'A' && c <= 'Z')
{
const char *code = morse_codes[c - 'A'];
printf("Wysylam: %c [%s]\n", c, code);

for (int j = 0; code[j] != '\0'; j++)
{
if (code[j] == '.')
{
send_pulse(target_pid, 1); // 1s = Kropka
}
else if (code[j] == '-')
{
send_pulse(target_pid, 2); // 2s = Kreska
}
}
// Koniec znaku
send_pulse(target_pid, 3);
}
else if (c == ' ')
{
printf("Wysylam: SPACJA\n");
send_pulse(target_pid, 4);
}
}
send_pulse(target_pid, 3);
printf("Wpisz kolejny tekst: ");
}

return 0;
}




receiver #include #include #include #include #include #include
char morse_buffer[10] = "";
time_t last_signal_time = 0;

const char *morse_codes[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
"-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
"..-", "...-", ".--", "-..-", "-.--", "--.."};
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void decode_and_print()
{
if (strlen(morse_buffer) == 0)
return;

int found = 0;
for (int i = 0; i < 26; i++)
{
if (strcmp(morse_buffer, morse_codes[i]) == 0)
{
printf("%c", alphabet[i]);
found = 1;
break;
}
}
if (!found)
{
printf("?");
}
fflush(stdout);
morse_buffer[0] = '\0';
}
void handle_signal(int sig)
{
time_t current_time = time(NULL);
if (last_signal_time == 0)
{
last_signal_time = current_time;
return;
}

double diff = difftime(current_time, last_signal_time);
last_signal_time = current_time;


if (diff >= 0.5 && diff < 1.5)
{
strcat(morse_buffer, ".");
}
else if (diff >= 1.5 && diff < 2.5)
{
strcat(morse_buffer, "-");
}
else if (diff >= 2.5 && diff < 3.5)
{
decode_and_print();
}
else if (diff >= 3.5)
{
decode_and_print();
printf(" ");
fflush(stdout);
}
}

int main()
{
printf("PID Odbiorcy: %d\n", getpid());


signal(SIGUSR1, handle_signal);


while (1)
{
pause();
}
return 0;
}
