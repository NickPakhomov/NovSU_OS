#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/* Структура для выбора танка */
struct structTank
{
		int TankNation;
		int TankType;
		
		struct Rus_Tank
		{
			int HT;
			int MT;
			int LT;
			int TD;
		} RusTank;
				
		struct Ger_Tank
		{
			int HT;
			int MT;
			int LT;
			int TD;
		} GerTank;
} Tank;

/* Функция перевода структуры Tank в сетевой порядок байт */
struct structTank ss(struct structTank Tank);

/* Функция выбора танка */
struct structTank TankFilter();

/* Функция вывода выбранного танка */
struct structTank ShowChosenTank();

/* Разбиваем IP и Port */
void IpAndPort(char *ipport, int *port);

int main(int argv, char *argc[])
{
	if (argv != 2)
	{
		perror("Введите нужные аргументы: локальный IP и порт\n");
		exit(-1);
	}
	/* Receiving ip and port */
	char *ipport;
	ipport=argc[1];
	
	/* Socket descriptor */
	int sockfd;

	int msgLength;
	int port;
	int i;
	struct structTank Tank;
	
	char nickname[25];
	printf ("Введите свой ник: ");
	scanf ("%s", nickname);
    //fgets(nickname,25,stdin);
    printf("\nПриветствую, %s! Настало время выбрать свой танк:\n",nickname);
		
	/* Выбираем танк */
	Tank=TankFilter(); 
	ShowChosenTank();

	/* Dividing 2nd argument into IP-address and port */
	IpAndPort(ipport, &port);
	
	struct sockaddr_in server_addr;
	/* Creating TCP socket */
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error creating socket\n");
		exit(-1);
	}
	
	/* Filling server address structure */
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	/* Converting IP to network order */
	if (inet_aton(ipport,&server_addr.sin_addr) == 0)
	{
		perror("Invalid IP address\n");
		close(sockfd);
		exit(-1);
	}

	/* Connecting logical connection via TCP */
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		perror("Error connecting to the server\n");
		close(sockfd);
		exit(-1);
	}

	/* Sending message length to the server */
	int n = sizeof(Tank);
	printf ("Size of Tank structure = %d\n", n);
	if ((msgLength=write(sockfd, &n, sizeof(int))) < 0)
	{
		perror("Error writing message length to the server\n");
		close(sockfd);
		exit(-1);
	}
	{
	//Переводим структуру Tank в сетевой порядок байт
	Tank = ss(Tank);
	
	/* Передача структуры выбранного танка */
	msgLength=write(sockfd, &Tank, sizeof(Tank));
	}
		if (msgLength < 0)
		{
			perror("Error sending data to the server\n");
			close(sockfd);
			exit(-1);
		}
	printf ("MsgLength = %d\n", msgLength);
	printf ("\n");
	
	msgLength=write(sockfd, &nickname, sizeof(nickname));

		if (msgLength < 0)
		{
			perror("Error sending Nickname to the server\n");
			close(sockfd);
			exit(-1);
		}
		
		
	int udpsockfd=0;
	//struct sockaddr_in servaddr, cliaddr; /* Структуры для
    //адресов сервера и клиента */
	struct sockaddr_in udpservaddr, udpcliaddr;
	 /* Создаем UDP сокет */
    if((udpsockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Не удалось создать UDP сокет"); 
        exit(1);
    }
	printf ("UDPSockFD = %d \n", udpsockfd);
    /* Заполняем структуру для адреса клиента: семейство 
    протоколов TCP/IP, сетевой интерфейс – любой, номер порта 
    по усмотрению операционной системы. Поскольку в структуре
    содержится дополнительное не нужное нам поле, которое 
    должно     быть нулевым, перед заполнением обнуляем ее всю */
    bzero(&udpcliaddr, sizeof(udpcliaddr));
    udpcliaddr.sin_family = AF_INET;
    udpcliaddr.sin_port = htons(0);
    udpcliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Настраиваем адрес сокета */
    if(bind(udpsockfd, (struct sockaddr *) &udpcliaddr, 
    sizeof(udpcliaddr)) < 0){
        perror("Не удалось связать UDP сокет");
        close(udpsockfd); /* По окончании работы закрываем 
        дескриптор сокета */
        exit(1);
    }
		/* Заполняем структуру для адреса сервера: 
	семейство протоколов TCP/IP, сетевой интерфейс – из аргумента
	командной строки, номер порта 7. Поскольку в 
	структуре содержится дополнительное не нужное нам
	поле, которое должно быть нулевым, перед заполнением
	обнуляем ее всю */
    bzero(&udpservaddr, sizeof(udpservaddr));
    udpservaddr.sin_family = AF_INET;
    udpservaddr.sin_port = htons(51000);
	printf ("Ipport = %s \n", ipport);
    if (inet_aton(ipport,&server_addr.sin_addr) == 0)
	{
        printf("Invalid IP address\n");
        close(udpsockfd); /* По окончании работы закрываем 
            дескриптор сокета */
        exit(1);
    }
	char coordinates[255];
	while(1)
	{
		if((n = recvfrom(udpsockfd, coordinates, 255, 0, 
		(struct sockaddr *) &udpservaddr, NULL)) < 0){
			perror("Error with recvfrom");
			close(udpsockfd);
			exit(1);
		}
		/* Печатаем пришедший ответ и закрываем сокет */
		printf("%s\n", coordinates);
		sleep(2);
		
	}
	close(sockfd);

	return 0;        
}

struct structTank ss(struct structTank Tank) {
	int *atank = (int*)&Tank;
	int count = sizeof(Tank) / sizeof(int);
	struct structTank out;
	int *aout = (int*)&out;
	int i;
	for (i = 0 ; i < count ; i++)
		aout[i] = htons(atank[i]);
		
	return out;
}

struct structTank TankFilter()
{
	struct structTank Tank;
	printf("Введите нацию:\n");
	printf (" 1. СССР \n 2. Германия\n");
	scanf ("%d", &Tank.TankNation);
	switch(Tank.TankNation)
	{
		//СССР
		case 1: 
			printf ("Введите тип танка: \n 1. Тяжелый танк \n 2. Средний танк \n 3. Легкий танк \n 4. ПТ-САУ \n");
			scanf ("%d", &Tank.TankType);
			switch(Tank.TankType)
			{
				case 1:
					printf ("Выберите Тяжелый танк: \n 1. ИС-3 \n 2. ИС-6 \n 3. КВ-4 \n 4. КВ-5 \n");
					scanf ("%d", &Tank.RusTank.HT);
				break;
				
				case 2:
					printf ("Выберите Средний танк: \n 1. Объект 416 \n 2. Т-44 \n");
					scanf ("%d", &Tank.RusTank.MT);
				break;

				case 3:
					printf ("Выберите Легкий танк: \n 1. МС-1 \n 2. Т-50 \n 3. МТ-25 \n");
					scanf ("%d", &Tank.RusTank.LT);
				break;	
				
				case 4:
					printf ("Выберите ПТ-САУ: \n 1. ИСУ-152 \n 2. СУ-101 \n");
					scanf ("%d", &Tank.RusTank.TD);
				break;

				default:
					break;
			}
		break;
		
		
		//Германия
		case 2: 
			printf ("Введите тип танка: \n 1. Тяжелый танк \n 2. Средний танк \n 3. Легкий танк \n 4. ПТ-САУ \n");
			scanf ("%d", &Tank.TankType);
			switch(Tank.TankType)
			{
				case 1:
					printf ("Выберите Тяжелый танк: \n 1. Tiger II \n 2. VK 45.02 (P) Ausf. A \n 3. Löwe \n");
					scanf ("%d", &Tank.GerTank.HT);
					break;
				
				case 2:
					printf ("Выберите Средний танк: \n 1. Indien-Panzer \n 2. Panther II \n");
					scanf ("%d", &Tank.GerTank.MT);
					break;
					
				case 3:
					printf ("Выберите Легкий танк: \n 1. Leichttraktor \n 2. Pz.Kpfw. I Ausf. C \n 3. VK 28.01 \n 4. Aufklärungspanzer Panther \n");
					scanf ("%d", &Tank.GerTank.LT);
					break;
				
				case 4:
					printf ("Выберите ПТ-САУ: \n 1. Ferdinand \n 2. Jagdpanther II \n 3. 8,8 cm Pak 43 Jagdtiger \n 4. Rhm.-Borsig Waffenträger \n");
					scanf ("%d", &Tank.GerTank.TD);
					break;
				default: 
					break;
			}
		break;
		
		default:
			break;
	}
	return Tank;
}

struct structTank ShowChosenTank()
{
	struct structTank Tank;
	switch(Tank.TankNation)
	{
		//СССР
		case 1: 
			
			switch(Tank.TankType)
			{
				case 1:
					switch(Tank.RusTank.HT)
						{
							case 1:
								printf ("Вы выбрали: ИС-3\n");
								break;
							case 2:
								printf ("Вы выбрали: ИС-6\n");
								break;
							case 3:
								printf ("Вы выбрали: КВ-4\n");
								break;
							case 5:
								printf ("Вы выбрали: КВ-5\n");
								break;
							default:
								printf ("Неправильный выбор Тяжелого танка СССР\n");
								break;
						}
				break;
				
				case 2:
					switch(Tank.RusTank.MT)
						{
							case 1:
								printf ("Вы выбрали: Объект 416\n");
								break;
							case 2:
								printf ("Вы выбрали: Т-44\n");
								break;
							default:
								printf ("Неправильный выбор Среднего танка СССР\n");
								break;
						}
				break;

				case 3:
					switch(Tank.RusTank.LT)
						{
							case 1:
								printf ("Вы выбрали: МС-1\n");
								break;
							case 2:
								printf ("Вы выбрали: Т-50\n");
								break;
							case 3:
								printf ("Вы выбрали: МТ-25\n");
								break;
							default:
								printf ("Неправильный выбор Легкого танка СССР\n");
								break;
						}
				break;	
				
				case 4:
					switch(Tank.RusTank.TD)
						{
							case 1:
								printf ("Вы выбрали: ИСУ-152\n");
								break;
							case 2:
								printf ("Вы выбрали: СУ-101\n");
								break;
							default:
								printf ("Неправильный выбор ПТ-САУ\n");
								break;
						}
				break;

				default:
					printf ("Неправильный выбор танка СССР\n");
					break;
			}
		break;
		
		
		//Германия
		case 2: 
			switch(Tank.TankType)
			{
				case 1:
					switch(Tank.GerTank.HT)
						{
							case 1:
								printf ("Вы выбрали: Tiger II\n");
								break;
							case 2:
								printf ("Вы выбрали: VK 45.02 (P) Ausf. A\n");
								break;
							case 3:
								printf ("Вы выбрали: Löwe\n");
								break;
							default:
								printf ("Неправильный выбор Тяжелого танка Германии\n");
								break;
						}
					break;
				
				case 2:
					switch(Tank.GerTank.MT)
						{
							case 1:
								printf ("Вы выбрали: Indien-Panzer\n");
								break;
							case 2:
								printf ("Вы выбрали: Panther II\n");
								break;
							default:
								printf ("Неправильный выбор Среднего танка Германии\n");
								break;
						}
					break;
					
				case 3:
					switch(Tank.GerTank.LT)
						{
							case 1:
								printf ("Вы выбрали: Leichttraktor\n");
								break;
							case 2:
								printf ("Вы выбрали: Pz.Kpfw. I Ausf. C\n");
								break;
							case 3:
								printf ("Вы выбрали: VK 28.01\n");
								break;
							case 4:
								printf ("Вы выбрали: Aufklärungspanzer Panther\n");
								break;
							default:
								printf ("Неправильный выбор Легкого танка Германии\n");
								break;
						}
					break;
				
				case 4:
					switch(Tank.GerTank.TD)
						{
							case 1:
								printf ("Вы выбрали: Ferdinand\n");
								break;
							case 2:
								printf ("Вы выбрали: Jagdpanther II\n");
								break;
							case 3:
								printf ("Вы выбрали: 8,8 cm Pak 43 Jagdtiger\n");
								break;	
							case 4:
								printf ("Вы выбрали: Rhm.-Borsig Waffenträger\n");
								break;	
							default:
								printf ("Неправильный выбор Легкого танка Германии\n");
								break;
						}
					break;
				default: 
					printf ("Неправильный выбор танка Германии\n");
					break;
			}
		break;
		
		default:
			printf ("Неправильный выбор нации \n");
			break;
	}
}

void IpAndPort(char *ipport, int *port)
{
	int i=0, j, n=0;
	char buf[10];
	while (ipport[i] != ':' && i < strlen(ipport)) i++;
	
	if (i == strlen(ipport))
	{
		printf("There's no port\n");
		exit(-1);
	}

	for (j = i+1; j < strlen(ipport); j++)
	{
		buf[n] = ipport[j];
		n++;
	}
	ipport[i] = 0;
	*port = atoi(buf);
}