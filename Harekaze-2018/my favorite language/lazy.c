#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char flag_chars[] = "0123456789ABCDEF";

int main()
{
	char *cmd = "echo -n HarekazeCTF{%s | ./lazy2_mod foo.lazy",
		 cmd_buffer[128],
		 flag[64] = {0};

	int flag_char_high = 0,
		news_high = 0;

	char buf[16];
	FILE *p;

	fputs("HarekazeCTF{", stdout);
	fflush(stdout);
	for(int i = 0;;i++)
	{
		for(int f = 0;f < 16;f++)
		{
			flag[i] = flag_chars[f];
			sprintf(cmd_buffer, cmd, flag);
			if((p = popen(cmd_buffer, "r")) == NULL)
			{
				fputs("\n", stdout);
				return -1;
			}

			if(fgets(buf, 16, p) != NULL)
			{
				int news = atoi(buf);

				if(news > news_high)
				{
					flag_char_high = f;
					news_high = news;
				}
			}
			if(pclose(p))
			{
				fputs("\n", stdout);
				return -1;
			}
		}
		putchar(flag_chars[flag_char_high]);
		fflush(stdout);

		flag[i] = flag_chars[flag_char_high];
		flag_char_high = 0,
		news_high = 0;
	}
}