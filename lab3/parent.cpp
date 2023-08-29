#define MAX_AMOUNT_OF_PROCESSES 5

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <ncurses.h>
#include<sys/select.h>
#include<curses.h>

#include <vector>
#include <iostream>

using namespace std;

vector<pid_t> procInfo; //вектор с ид дочерних процессов
struct sigaction printSignal; 
bool Print = true;
bool printAvailable[] = {true, true, true, true, true}; //массив доступности доч процессов
char parentPID[256];
int i = 0; //индекс доч ид в векторе

int myGetch() //получение символа без ожидания enter
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void setPrint(int sig) //проверяет procInfo если SIGUSR1 для разрешения на вывод
{
    if (procInfo.size()) //проверка сущ доч процесса
    {
        if (++i >= procInfo.size()) 
        {
            i = 0;
        }
        int j = 0;
        while(printAvailable[i] != true) //проверяет выход за предел массива
        {
            if (++i >= procInfo.size())
            {
                i = 0;
            }
            j++;
            if(j == MAX_AMOUNT_OF_PROCESSES){
                Print = false;
                break;}
        }
        napms(150);
        if(Print == true)
            kill(procInfo[i], SIGUSR1); //доч процессу отпр сигнал SIGUSR1
    }
    else
    {
        Print = true;
    }
}

void initSignalHandlers() //инициализация пользоват сигнала для упрвл доч процессами
{
    printSignal.sa_handler = setPrint; //ссылка на функцию setPrint
    printSignal.sa_flags =  SA_RESTART; //флаг для повторного запуска дочернего процесса
    sigaction(SIGUSR1, &printSignal, NULL); //устанавливает printSignal в пользвл сигнал SIGUSR1
}

void addOneProcess() {
    procInfo.push_back(fork()); //добавл ид доч процесса в procInfo

    if (procInfo.back() == 0) //если доч процесс выполняется
    {
        char instanceID[10]; //номер заданного процесса
        sprintf(instanceID, "%ld", procInfo.size()); //запись номера ид в instanceID
        if (execlp("./child", instanceID, parentPID, NULL) == -1) //ошибка создания дочернего процесса
        {
            procInfo.pop_back();
            cout << "Error." << endl;
        }
    }
}
/*Новый процесс может не запуститься по нескольким причинам:
1. Ошибка в вызове функции fork(). Например, если система не может создать новый процесс из-за ограничений в ресурсах.
2. Ошибка в вызове функции execlp(). Например, если программа, которую вы пытаетесь запустить, не существует или у вас нет прав на ее запуск.
3. Нехватка ресурсов. Например, если система не может выделить достаточно памяти для нового процесса.
4. Ошибка в коде программы, которая запускается. Например, если программа содержит ошибки сегментации или деления на ноль.
5. Ошибка в конфигурации системы. Например, если в системе не настроены необходимые библиотеки или необходимые файлы не существуют.*/

void removeOneProcess()
{
    kill(procInfo.back(), SIGUSR2);//доч процессу отпр сигнал SIGUSR1
    waitpid(procInfo.back(), NULL, 0); //ожидание завершения процесса

    printf("Process %d was deleted \n", procInfo.back());

    procInfo.pop_back(); //удалие идентификатора из procInfo

    printf("%ld left", procInfo.size());
}

void showOptionList() //меню
{
    cout << "'+' to create new child;" << endl;
    cout << "'-' to delete last child;" << endl;
    cout << "'l' to show all active processes;" << endl;
    cout << "'k' to delete all childs;" << endl;
    cout << "'s' to disable displaying statistics;" << endl;
    cout << "'g' to allow displaying statistics;" << endl;
    cout << "'p' to show one process during 5 seconds;" << endl;
    cout << "'q' to quit;\n" << endl;
}

void ShowProcesses()//отобразить список процессов
{
    printf("Parent process: %d \n", getpid()); //вывод род процесса
    int k = 1;
    for(auto j: procInfo) //вывод дочерних процессов
    {
        printf("%d.Child process: %d \n", k++, j);
    }
}

int main()
{

    showOptionList(); //отобразить меню

    initSignalHandlers(); //инициализация пользоват сигнала для упрвл доч процессами
    sprintf(parentPID, "%d", getpid()); //вывод род ид


    while (true) //цикл ввода выбора меню
    {
        switch(getchar())
        {
            case '+':
            {
                if(procInfo.size() < MAX_AMOUNT_OF_PROCESSES)
                {
                    addOneProcess(); //добавить доч процесс
                    if (Print == true) 
                    {
                        napms(150);
                        kill(procInfo.back(), SIGUSR1);//доч процессу отпр сигнал SIGUSR1
                    }
                }
                break;
            }
            case '-':
                {
                if (!procInfo.empty())
                {
                    if (i == procInfo.size() - 1) //если процесс последний в списке был удален 
                    {
                        removeOneProcess();
                        raise(SIGUSR1); //возбуждается сигнал SIGUSR1
                    }
                    else
                    {
                        removeOneProcess(); //удаление процесса
                    }
                }
                break;
            }
            case 'l':
            {
                ShowProcesses(); //отобразить список процессов
                break;
            }
            case 'k':
            {
                while (!procInfo.empty()) //удалить все процессы
                {
                    removeOneProcess();
                }
                raise(SIGUSR1);//возбуждается сигнал SIGUSR1
                break;
            }
            case 's':
            {
                printf("\n\n\nParametr (1, 2, 3, 4, 5) \n");
                switch(getchar())
                {
                    case '1':{printAvailable[0]=false; break;}
                    case '2':{printAvailable[1]=false; break;}
                    case '3':{printAvailable[2]=false; break;}
                    case '4':{printAvailable[3]=false; break;}
                    case '5':{printAvailable[4]=false; break;}
                    default: {Print = false; //запрет на вывод для всех процессов
                    for (int j = 0; j<MAX_AMOUNT_OF_PROCESSES; j++)
                        {
                            printAvailable[j]=false;
                        }  
                     break;}
                }
                break;
            }
            case 'g':
            {
                printf("\n\n\nParametr (1, 2, 3, 4, 5) \n");
                Print = true;
                switch(myGetch())
                {
                    
                    case '1':{printAvailable[0]=true; break;}
                    case '2':{printAvailable[1]=true; break;}
                    case '3':{printAvailable[2]=true; break;}
                    case '4':{printAvailable[3]=true; break;}
                    case '5':{printAvailable[4]=true; break;}
                    default: { //разрешить для всех
                        
                        for (int j = 0; j<MAX_AMOUNT_OF_PROCESSES; j++)
                        {
                            printAvailable[j]=true;
                        }    
                        break;
                    }
                }
                raise(SIGUSR1);
                break;
            }
            case 'p':
            {
                for (int j = 0; j<MAX_AMOUNT_OF_PROCESSES; j++) //запретить для всех процессов
                {
                    printAvailable[j]=false;
                }  
                switch(myGetch()) //разрешить для выбранного, если не выбрано запрещается всем
                {
                   case '1':{printAvailable[0]=true; break;}
                    case '2':{printAvailable[1]=true; break;}
                    case '3':{printAvailable[2]=true; break;}
                    case '4':{printAvailable[3]=true; break;}
                    case '5':{printAvailable[4]=true; break;}
                }
                raise(SIGUSR1);
                napms(4000); //задержка на 4с
                 Print = true;
                  for (int j = 0; j<MAX_AMOUNT_OF_PROCESSES; j++) //разрешить всем снова
                        {
                            printAvailable[j]=true;
                        }  
                break;
            }
            case 'q':
                {
                while (!procInfo.empty())
                {
                    removeOneProcess(); //удалить все процессы
                }
                cout << "\nPress any key to proceed...\n";
                myGetch();
                return 0; //завершить родительскую программу
            }
        }
    }

    return 0;
}