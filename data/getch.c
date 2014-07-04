//Taken from:
// http://stackoverflow.com/questions/7499846/convert-getch-response-to-ascii-character
// Use this to redefine your data/keyboard.def if you find the keys are not working

#include <ncurses.h>
#include <cctype>

int main(int ac, char **av) 
{
    WINDOW* mainWin(initscr());
    cbreak();
    noecho();

    // Invoke keypad(x, true) to ensure that arrow keys produce KEY_UP, et al,
    // and not multiple keystrokes.
    //keypad(mainWin, true);

    mvprintw(0, 0, "press a key: ");
    int ch;

    // Note that getch() returns, among other things, the ASCII code of any key
    // that is pressed. Notice that comparing the return from getch with 'q'
    // works, since getch() returns the ASCII code 'q' if the users presses that key.
    while( ch = getch() ) {
      erase();
      move(0,0);
      if(isascii(ch)) {
        if(isprint(ch)) {
          // Notice how the return code (if it is ascii) can be printed either
          // as a character or as a numeric value.
          printw("You pressed a printable ascii key: %d\n", ch, ch);
        } else {
          printw("You pressed an unprintable ascii key: %d\n", ch);
        }
      }

      // Again, getch result compared against an ASCII value: '\t', a.k.a. 9
      if(ch == '\t') {
        printw("You pressed tab.\n");
      }
      printw("Press another key, or 'ctr+c' to quit\n");
      refresh();
    }

    endwin();
}
