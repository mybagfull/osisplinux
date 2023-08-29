#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ncurses.h>

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

struct sigaction printSignal, closeSignal, alarmSignal; //структуры сигналов
bool Print = false;
bool EndAlarm = false;
bool Close = false;
char parentPID[256];
int result = 0;

struct TimerStat //структура таймера
{
    int first;
    int second; 

}timeStat;

struct Stat //структура статистики доч процесса
{
    int res00 = 0;
    int res01 = 0;
    int res10 = 0;
    int res11 = 0; 
}all_stat;

void setPrint(int sign)//printSignal
{
    Print = true;
}

void setClose(int sign)//closeSignal
{
    Close = true;
}

void setAlarm (int sign)
{     
    EndAlarm = true;
    if(timeStat.first == timeStat.second) //расчеты для статистики
    {
        if(timeStat.first == 1){
            all_stat.res11++;
        }
        else{
            all_stat.res00++;
        }
    }
    else{
        if(timeStat.first ==0){
            all_stat.res01++;
        }
        else{
            all_stat.res10++;
        }
    }
    
} 

void initSignalHandlers() //инициализация польз сигналов
{
    printSignal.sa_handler = setPrint; //printSignal отвечает за сигнал SIGUSR1, 
    sigaction(SIGUSR1, &printSignal, NULL);//который будет послан родительским процессом для печати статистики работы дочернего процесса

    closeSignal.sa_handler = setClose;//closeSignal отвечает за сигнал SIGUSR2, 
    sigaction(SIGUSR2, &closeSignal, NULL);//который будет послан родительским процессом для завершения дочернего процесса

    alarmSignal.sa_handler = setAlarm;
    sigaction(SIGALRM, &alarmSignal, NULL);
    /*alarmSignal отвечает за сигнал SIGALRM, который будет генерироваться каждые 10 миллисекунд с помощью функции ualarm(). 
    Обработчик setAlarm установит флаг EndAlarm, который будет сигнализировать о том, что прошло достаточное время. Кроме того, 
    при обработке сигнала setAlarm происходит подсчет статистики работы дочернего процесса и сохранение ее в глобальную структуру all_stat.*/
}

int main(int argc, char* argv[])
{

    printf("New child process pid: %d, ppid: %d  \n", getpid(), getppid()); //вывод параметров
    initSignalHandlers();

    char buf[256]; //буфер для вывода статистики
    int count = 100;
    while (count --)
    {
        bool fl;
        EndAlarm = false;
        ualarm(10000, 0); //таймер на 10 мс
        while(!EndAlarm)
        {
            timeStat.first = fl ? 1:0; //если true то 1, иначе 0
            timeStat.second = fl ? 1:0;//если true то 1, иначе 0
            fl = fl ? false : true;
        }
    }

    sprintf(buf, " Child%d 00: %d 01:%d, 10:%d, 11:%d", atoi(argv[0]), all_stat.res00, all_stat.res01, all_stat.res10, all_stat.res11);
    //загрузка статистики в буфер

    while (true)
    {
        if (Print)
        {
            usleep(10000); 
            Print = false;                
            cout << buf << endl;
            napms(300);
            
            kill(getppid(), SIGUSR1);//род процессу отпр сигнал SIGUSR1
        }
        
        if (Close) //завершить если пришел сигнал 
        {
            break;
        }
    }


    return 0;
}





/*xSignal - это механизм, который позволяет процессам взаимодействовать друг с другом и с ядром операционной системы. 
Когда процесс получает сигнал, он может выполнить некоторое действие, связанное с этим сигналом, например, 
завершить свою работу или изменить свое поведение.
Вот некоторые из наиболее распространенных сигналов, которые используются в UNIX-подобных операционных системах, и их предназначение:

SIGTERM - сигнал, который посылается процессу для запроса его завершения. 
Этот сигнал может быть перехвачен процессом для выполнения дополнительных действий перед завершением работы.

SIGKILL - сигнал, который принудительно завершает работу процесса без возможности его перехвата или обработки. 
Этот сигнал используется в тех случаях, когда необходимо немедленно завершить работу процесса.

SIGUSR1 и SIGUSR2 - пользовательские сигналы, которые могут быть использованы приложениями для передачи пользовательских команд процессу.

SIGALRM - сигнал, который посылается процессу при истечении таймера, который был установлен с помощью функции alarm(). 
тот сигнал может использоваться для выполнения определенных действий по истечении заданного времени.

sigaction - это функция, которая позволяет установить обработчик сигнала для процесса. 
Обработчик сигнала - это функция, которая будет вызвана при получении указанного сигнала процессом.

kill - это функция, которая используется для посылки сигнала процессу или группе процессов.*/