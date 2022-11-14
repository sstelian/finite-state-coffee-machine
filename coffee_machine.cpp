// include common libraries
#include <iostream>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#if defined (__linux__) // include Linux libraries
  #include <chrono>
  #include <termios.h>
  using namespace std::this_thread;
#elif defined (_WIN32) // include Windows libraries
  #include <conio.h>
  #include <windows.h>
#else
  #error "Platform not supported."
#endif

#define ACTION_DURATION 20
using namespace std;


typedef enum
{
  STATUS_OK,
  STATUS_PNDING,
  STATUS_FAILED
} status_t;

typedef enum
{
  PREHEAT,
  IDLE,
  HEAT,
  ENABLE_PUMP,
  CLEANUP
} state_t;

// Linux implementation for kbhit
#if defined (__linux__)
  int kbhit_linux()
  {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
      ungetc(ch, stdin);
      return 1;
    }

    return 0;
  }
#endif

void sleepMs(int d)
{
  #if defined (__linux__) // call Linux implementation
    sleep_for(std::chrono::milliseconds(d));
  #elif defined (_WIN32) // call Windows implementation
    Sleep(d);
  #else
    #error "Platform not supported."
  #endif
}


int kbhitWrapper()
{
  #if defined (__linux__) // call Linux implementation
    return kbhit_linux();
  #elif defined (_WIN32) // call Windows implementation
    return _kbhit();
  #else
    #error "Platform not supported."
  #endif
}

// state functions
status_t preheat()
{
  status_t status = STATUS_OK;

  cout << "Preheating. Press F to cause a failure." << endl;

  for (int i = 0; i < ACTION_DURATION; i++)
  {
    sleepMs(100);

    if (!kbhitWrapper())
    {
      continue;
    }

    int c = getchar();
    if ((c == 'f') || (c == 'F'))
    {
      status = STATUS_FAILED;

      break;
    }
  }

  return status;
}

status_t idle()
{
  status_t status = STATUS_PNDING;

  cout << "Idling. Press C to make a coffee." << endl;

  while (1)
  {
    if (!kbhitWrapper())
    {
      continue;
    }

    int c = getchar();
    if ((c == 'c') || (c == 'C'))
    {
      status = STATUS_OK;

      break;
    }
  }

  return status;
}

status_t heat()
{
  status_t status = STATUS_OK;

  cout << "Heating up. Press F to cause a failure." << endl;

  for (int i = 0; i < ACTION_DURATION; i++)
  {
    sleepMs(100);

    if (!kbhitWrapper())
    {
      continue;
    }

    int c = getchar();
    if ((c == 'f') || (c == 'F'))
    {
      status = STATUS_FAILED;

      break;
    }
  }

  return status;
}

status_t enablePump()
{
  status_t status = STATUS_OK;

  cout << "Pumping water. Press F to cause a failure." << endl;

  for (int i = 0; i < ACTION_DURATION; i++)
  {
    sleepMs(100);

    if (!kbhitWrapper())
    {
      continue;
    }

    int c = getchar();
    if ((c == 'f') || (c == 'F'))
    {
      status = STATUS_FAILED;

      break;
    }
  }

  return status;
}

status_t cleanup()
{
  status_t status = STATUS_OK;

  cout << "Cleaning up. Press F to cause a failure." << endl;

  for (int i = 0; i < ACTION_DURATION; i++)
  {
    sleepMs(100);

    if (!kbhitWrapper())
    {
      continue;
    }

    int c = getchar();
    if ((c == 'f') || (c == 'F'))
    {
      status = STATUS_FAILED;

      break;
    }
  }

  return status;
}

int main()
{
  state_t state = PREHEAT;
  status_t status = STATUS_FAILED;

  while(1)
  {
    // state machine implementation
    switch(state)
    {
      case PREHEAT:
        status = preheat();

        // if current action was successful, move on to the next action
        if (status == STATUS_OK)
        {
          state = IDLE;
        } else
        if (status == STATUS_FAILED) // basic error handling
        {
          cout << endl <<"Preheat failed" << endl;

          state = CLEANUP;
        }
      break;

      case IDLE:
        status = idle();

        // only move on to the next action if the user requested a coffee
        if (status == STATUS_OK)
        {
          state = HEAT;
        }
        // let's assume idle cannot possibly fail
      break;

      case HEAT:
        status = heat();

        // if current action was successful, move on to the next action
        if (status == STATUS_OK)
        {
          state = ENABLE_PUMP;
        } else
        if (status == STATUS_FAILED) // basic error handling
        {
          cout << endl <<"HEAT failed" << endl;

          state = CLEANUP;
        }
      break;

      case ENABLE_PUMP:
        status = enablePump();

        // if current action was successful, move on to the next action
        if (status == STATUS_OK)
        {
          state = CLEANUP;
        } else
        if (status == STATUS_FAILED) // basic error handling
        {
          cout << endl <<"Enable pump failed" << endl;

          state = CLEANUP;
        }
      break;

      case CLEANUP:
        status = cleanup();

        // if current action was successful, move on to the next action
        if (status == STATUS_OK)
        {
          state = IDLE;
        } else
        if (status == STATUS_FAILED) // basic error handling
        {
          cout << endl <<"Cleanup failed" << endl;

          state = IDLE;
        }
      break;

      default:
        cout << "ERROR: Unknown state" << endl;

        state = IDLE;
      break;
    }
  }

  return 0;
}
