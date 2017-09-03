#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <float.h>


#define OPTION_MAX_SIZE 1
#define NAME_MIN_SIZE 6
#define NAME_MAX_SIZE 50
#define USERNAME_MIN_SIZE 6
#define USERNAME_MAX_SIZE 20
#define PASSWORD_MIN_SIZE 6
#define PASSWORD_MAX_SIZE 50
#define EMAIL_MAX_SIZE 50
#define CONTACT_SIZE_1 9
#define CONTACT_SIZE_2 12
#define MONEY_MAX_SIZE_STRING 42      //340282346638528859811704183484516925440.00 tem 42 caracteres
#define MONEY_MAX_SIZE_FLOAT 340282346638528859811704183484516925440.00
#define QUANTITY_MAX_SIZE_STRING 10
#define QUANTITY_MAX_SIZE_INT 2147483647
#define PERMS_MAX_SIZE 1
#define PERMS_MAX_SIZE_STRING 5      //ADMIN = 5 caracteres
#define CODE_SIZE 9
#define CONFIRMATION_MAX_SIZE 3
#define DAY_MAX_SIZE 2
#define MONTH_MAX_SIZE 2
#define YEAR_MAX_SIZE 4
#define FILE_MAX_SIZE 37      //8 ("userdb\") + 20 (username) + 8 (shoplist)
#define LOGGED_MAX_SIZE 5      //FALSE é o maior

/* Notas: 1 - Nas variáveis que contenham os nomes, usernames etc acima descritas, temos que meter mais 2 espaços
para verificação de erros e para variáveis onde vamos ler de ficheiros, é só mais 1 */


static char loggedUsername[USERNAME_MAX_SIZE+1];      //variável global para ver qual é o username que está com sessão iniciada
static char realDay[DAY_MAX_SIZE+2];
static char realMonth[MONTH_MAX_SIZE+2];
static char realYear[YEAR_MAX_SIZE+2];

//autenticação
bool auth();
void logUser(char username[]);
void logoutUser(char username[]);

//menus
void menu();      //menu principal
void menuManageAccount();      //menu de gestão de utilizadores
void menuChangeUser();      //menu de modificar propriedades dos utilizadores
void menuManageShoppingList();      //menu de gestão de stock
void menuStats();      //menu de estatisticas

//verificações: não foi posto a impressão de erros nas verificações por causa da palavra "Novo"
bool checkNewOption(char option[]);      //verificar uma opção introduzida
bool checkNewName(char username[]);      //verificar um novo nome
bool checkNewUsername(char username[]);      //verificar um novo username
bool checkNewPass(char password[]);      //verificar uma nova password
bool checkNewEmail(char email[]);      //verificar um novo e-mail
bool checkNewContact(char contact[]);      //verificar um novo contacto
bool checkNewBalance(char stringBalance[]);      //verificar um novo saldo de conta bancária
bool checkConfirmation(char confirmation[]);      //verificar confirmações
bool checkNewCode(char code[]);      //verificar códigos de produtos
bool checkNewQuantity(char stringQuantity[]);      //verificar quantidades de produtos
bool checkDay(char day[]);      //verificar o dia de uma data
bool checkMonth(char month[]);      //verificar mes de uma data
bool checkYear(char year[]);


//gerir utilizadores
struct userData {      //estrutura das propriedades de um utilizador
	char name[NAME_MAX_SIZE+1];      //+1 para o \0'
	char username[USERNAME_MAX_SIZE+1];
	char password[PASSWORD_MAX_SIZE+1];
	char email[EMAIL_MAX_SIZE+1];
	char contact[CONTACT_SIZE_2+1];
	float balance;
	char perms[PERMS_MAX_SIZE_STRING+1];
	char logged[LOGGED_MAX_SIZE+1];
};
void showBalance();
void registerUser();      //adicionar um novo utilizador
void changeName();      //modificar o nome de um utilizador
void changeContact();      //modificar o contactp de um utilizador
void changeEmail();      //modificar o e-mail de um utilizador
void changePass();      //modificar a password de um utilizador
void changeBalance();      //modificar o saldo de uma conta bancária de um utilizador
void deleteAccount();      //eliminar um utilizador

//gerir produtos
struct productData {      //estrutura das propriedades de um produto
	char code[CODE_SIZE+1];      //+1 para o '\0'
	char name[NAME_MAX_SIZE+1];
	int quantity;
	float price;
};
int showCart();
void searchProduct();      //listar todos os produtos existentes
void buyIndividualProduct();
void addProductToCart();      //adicionar um novo produto ao carrinho
void changeCart();      //modificar produtos no carrinho
void deleteProductFromCart();      //eliminar um produto do carrino
void checkout();
struct shoplistData {
	char code[CODE_SIZE+1];
	int quantity;
};

//estatisticas
struct salesData {
	char code[CODE_SIZE+1];
	int quantity;
	char username[USERNAME_MAX_SIZE+1];
	char day[DAY_MAX_SIZE+1];
	char month[MONTH_MAX_SIZE+1];
	char year[YEAR_MAX_SIZE+1];
};
void productStats();      //estatisticas de um produto
void topStats();      //TOP estatísticas de compras
void dailyStats();      //estatisticas diárias de vendas
void topDailyStats();      //TOP produtos vendidos num determinado dia
void monthlyStats();      //estatisticas mensais de vendas
void topMonthlyStats();      //TOP produtos vendidos num determinado mes

//funções auxiliares
//estrutura das propriedades de um ficheiro (propriedades que é preciso passar em várias funções)
struct fileStructure {
	int fd;
	struct flock *lock;
	FILE *filePointer;
};

/*abrir um ficheiro especificado com os modos READ, WRITE ou APPEND (ler, escrever ou acrescentar ao fim, respetivamente)
e meter um lock de leitura, escrita ou escrita para mais ninguem poder aceder ao ficheiro
Lock de leitura = quem quiser pode ler mas nã\o escrever
Lock de escrita = só quem tem o lock pode escrever e ninguem pode ler
Lock de acrescentar = Lock de escrita*/
struct fileStructure openFile(char fileToEdit[], char mode[]);
void closeFile(struct fileStructure fileStruct);      //fechar e retirar o lock do ficheiro antes aberto

/* https://bytes.com/topic/c/answers/726898-checking-excessive-input-when-using-fgets */
void flushInput();      //limpar o buffer de input (maneira em que dá para todos os sistemas operativos)
bool getString(char string[], int size);      //fgets mas eliminar o que resta no buffer stdin


void generateCode(char code[]);      //gerar código de produto aleatório

void cleanScreen();      //limpar o ecra (meter vários ENTER's ( '\n''s ))
void removeNewLine(char string[]);      //trocar o ENTER ('\n') pelo null ('\0') numa string para dar menos erros a comparar strings
float stringToFloat(char stringBalance[]);      //converter strings para floats
int stringToInt(char stringQuantity[]);

void about();      //imprimir informações sobre o programa



int main(){

	//criar o diretório "usersdb" caso não exista e caso exista ele não vai ser criado
	mkdir("usersdb", 0766);      //mkdir = comando para criar diretórios, 0 = octal, 7 = RWX(user), 6 = RW(group + otherusers)


	//criar os ficheiros "userdb", "productdb" e "salesdb" se não existirem
	struct fileStructure file_userdb = openFile("userdb", "READ");
	
	if (!file_userdb.filePointer)
		return -1;
	
	closeFile(file_userdb);
	

	struct fileStructure file_productdb = openFile("productdb", "READ");
	if (!file_productdb.filePointer)
		return -1;
	closeFile(file_productdb);

	struct fileStructure file_salesdb = openFile("salesdb", "READ");
	if (!file_salesdb.filePointer)
		return -1;
	closeFile(file_salesdb);

	fputs("-----Cliente Digital do SuperMercado-----\n\n", stdout);
	fputs("Recomendação: Usar em Fullscreen\n\n\n", stdout);

	fputs("---Inserir data---\n\n",stdout);
	fputs("Dia: ",stdout);
	getString(realDay, DAY_MAX_SIZE+2);
	if (!checkDay(realDay)){      //verificar se o dia é válido
		cleanScreen();
		fprintf(stdout, "Erro! Dia não válido.\n");
		return -1;
	}

	fputs("Mes: ",stdout);
	getString(realMonth, MONTH_MAX_SIZE+2);
	if (!checkMonth(realMonth)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n");
		return -1;
	}

	fputs("Ano: ",stdout);
	getString(realYear, YEAR_MAX_SIZE+2);
	fputs("\n\n", stdout);
	if (!checkYear(realYear)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n");
		return -1;
	}

	cleanScreen();      //limpar o ecra para ficar mais bonito
	char option[OPTION_MAX_SIZE+2]="0";      /*criar um array de caracteres com 1+2 espaços para [0] = opção,
												[1] = verificação de erros (ex: 10 = errado), [2] = '\0'*/

	while(option[0]!='4'){
		fputs("-----Autenticação-----\n\n", stdout);
		fputs("1) Login\n", stdout);
		fputs("2) Registar\n", stdout);
		fputs("3) Sobre\n", stdout);
		fputs("4) Sair\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';      /*como todas as opções sao algarismos, isto vai fazer 
								com que a opção não seja reconhecida*/

		switch(option[0]){
			case '1': cleanScreen();
				if ( auth() )      //se auth() retornar "true" então ir para o menu
					menu();
				break;
			case '2': cleanScreen();
				registerUser();
				break;
			case '3': cleanScreen();
				about();
				break;
			case '4': cleanScreen();
				fputs("Obrigado por escolher o SuperMercado e volte sempre!\n", stdout);
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
	

	//retornar 0 para saber se o programa terminou bem
	return 0;
}

bool auth(){
	//+2 para um caracter a mais para determinar se a string obedece a todas as propriedades e para o '\0'
	char username[USERNAME_MAX_SIZE+2], password[PASSWORD_MAX_SIZE+2];

	fputs("\n-----Login-----\n\n", stdout);

	fputs("Username: ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);
	fputs("Password: ", stdout);
	getString(password, PASSWORD_MAX_SIZE+2);


	/*abrir o ficheiro userdb, meter um lock (modo de leitura) no ficheiro e passar
	as informações para a estrutura file_userdb*/
	struct fileStructure file_userdb = openFile("userdb", "READ");

	//se o ficheiro não conseguiu ser aberto, dar erro e sair desta função
	if (!file_userdb.filePointer)
		return false;

	/*declarar variáveis a ser usadas nas operações de ficheiros 
	(só é declarado aqui em baixo para poupar memória caso o ficheiro não consiga ser aberto)*/
	char fileUsername[USERNAME_MAX_SIZE+1], filePassword[PASSWORD_MAX_SIZE+1], fileLogged[LOGGED_MAX_SIZE+1];      //+1 para '\0'

	//enquanto não lermos o ficheiro todo, continuar a ler
	/*  %[^;]; = ler qualquer coisa separado por ;
	    %*[^;]; ignorar (*) qualquer coisa separado por ; 
	http://stackoverflow.com/questions/20450333/fscanf-with-colon-delimited-data*/
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%[^;];%*[^;];%*[^;];%*f;%*[^;];%s\n", fileUsername, filePassword, fileLogged)!=EOF){

		if ( !strcmp(fileUsername, username)) {      //se o utilizador existir
			if ( !strcmp(filePassword, password)) {      //se o utilizador existir e a password estiver certa
				if ( !strcmp(fileLogged, "FALSE")){      //se o utilizador existir, a password estiver certa e ainda não se ter autenticado
					closeFile(file_userdb);
					strcpy(loggedUsername, username);
					logUser(username);
					cleanScreen();
					fputs("Autenticado com sucesso!\n\n\n", stdout);
					return true;
				}
				else {      //se o utilizador existir, a password estiver certa mas já estar autenticado
					closeFile(file_userdb);
					cleanScreen();
					fputs("Erro! Este utilizador já está autenticado.\n\n\n", stdout);
					return false;
				}

			}
			else {      //se o utilizador existir e a password NÃO estiver certa
				closeFile(file_userdb);
				cleanScreen();
				fputs("Erro! Password errada. Por-favor tente outra vez.\n\n\n", stdout);
				return false;
			}
		}
	}

	//se o utilizador não existir
	//fechar e tirar o lock do ficheiro
	closeFile(file_userdb);
	cleanScreen();
	fputs("Erro! Utilizador não encontrado. Por-favor tente outra vez.\n\n\n", stdout);
	return false;
}

void registerUser(){

	char name[NAME_MAX_SIZE+2], contact[CONTACT_SIZE_2+2] , email[EMAIL_MAX_SIZE+2], username[USERNAME_MAX_SIZE+2],
	password[PASSWORD_MAX_SIZE+2], stringBalance[MONEY_MAX_SIZE_STRING+2];

	fputs("-----Registar-----\n\n\n", stdout);
	fputs("* = obrigatório ser original\n\n", stdout);
	fputs("Nome completo (não pode conter ';' e tem que ter entre 6 a 50 caracteres): ", stdout);
	getString(name, NAME_MAX_SIZE+2);

	//verificar se todos os campos são válidos, senão sair sem escrever para o ficheiro
	if (!checkNewName(name)){
		cleanScreen();
		fputs("Erro! Nome não válido.\n\n\n", stdout);
		return;
	}

	fputs("Username (6 a 20 caracteres e não são permitidos ';' nem espaços)*: ", stdout);
	getString(username, USERNAME_MAX_SIZE+2);
	if (!checkNewUsername(username)){
		cleanScreen();
		fputs("Erro! Username não válido.\n\n\n", stdout);
		return;
	}

	
	fputs("Password (6 a 50 caracteres e não são permitidos ';' nem espaços): ", stdout);
	getString(password, PASSWORD_MAX_SIZE+2);
	if (!checkNewPass(password)){
		cleanScreen();
		fputs("Erro! Password não válida.\n\n\n", stdout);
		return;
	}


	fputs("Endereço de e-mail: (não pode conter ';' e ter no máximo 50 caracteres)*: ", stdout);
	getString(email, EMAIL_MAX_SIZE+2);
	if (!checkNewEmail(email)){
		cleanScreen();
		fputs("Erro! Endereço de e-mail não válido.\n\n\n", stdout);
		return;
	}


	fputs("Contacto telefónico (conter 9 ou 12 números)*: ", stdout);
	getString(contact, CONTACT_SIZE_2+2);
	if (!checkNewContact(contact)){
		cleanScreen();
		fputs("Erro! Contacto telefónico não válido.\n\n\n", stdout);
		return;
	}


	fputs("Saldo inicial da conta bancária (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(stringBalance, MONEY_MAX_SIZE_STRING+2);
	if (!checkNewBalance(stringBalance)){
		cleanScreen();
		fputs("Erro! Saldo inicial da conta bancária não válido.\n\n\n", stdout);
		return;
	}


	//abrir o ficheiro e meter um lock (modo de acrescentar)
	struct fileStructure file_userdb = openFile("userdb", "APPEND");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], fileEmail[EMAIL_MAX_SIZE+1], fileContact[CONTACT_SIZE_2+1];
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%[^;];%[^;];%*f;%*[^;];%*s\n", fileUsername, fileEmail, fileContact)!=EOF){
		if ( !strcmp(fileUsername, username)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o username \"%s\" já existe.\n\n\n", username);
			return;
		}
		else if ( !strcmp(fileEmail, email)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o e-mail \"%s\" já existe.\n\n\n", email);
			return;
		}
		else if ( !strcmp(fileContact, contact)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o contacto telefónico \"%s\" já existe.\n\n\n", contact);
			return;
		}
	}

	float balance = stringToFloat(stringBalance);
	fprintf(file_userdb.filePointer, "%s;%s;%s;%s;%s;%f;USER;FALSE\n", name, username, password, email, contact, balance);

	closeFile(file_userdb);


	//variaveis para concatenar
	char createDir[FILE_MAX_SIZE+1]="usersdb/";      //8 (usersdb/) + 20 (USERNAME_MAX_SIZE) + 1 ('\0')
	strcat(createDir, username);

	//criar diretório de utilizador
    mkdir(createDir, 0766);      //mkdir = comando para criar diretórios


	char createShoplist[FILE_MAX_SIZE+1]="usersdb/";      //28 = diretório + 8 (shoplist) + 1 ('\0')
	strcat(createShoplist, username);
	strcat(createShoplist, "/");
	strcat(createShoplist, "shoplist");

    //criar ficheiro de lista de compras
    struct fileStructure file_create = openFile(createShoplist, "READ");
	if (!file_create.filePointer)
		return;
	closeFile(file_create);
	
	
	//limpar o ecra
	cleanScreen();
	fprintf(stdout, "Utilizador \"%s\" criado com sucesso!\n\n\n", username);
}

void menu(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='4'){
		fputs("-----Menu-----\n\n", stdout);
		fputs("1) Gerir conta\n", stdout);
		fputs("2) Menu de compras\n", stdout);
		fputs("3) Ver Estatísticas\n", stdout);
		fputs("4) Logout\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';      /*como todas as opções sao algarismos, isto vai fazer 
								com que a opção não seja reconhecida*/

		switch(option[0]){
			case '1': cleanScreen();
				menuManageAccount();
				break;
			case '2': cleanScreen();
				menuManageShoppingList();
				break;
			case '3': cleanScreen();
				menuStats();
				break;
			case '4': cleanScreen();
				logoutUser(loggedUsername);
				fputs("Logout!\n\n\n", stdout);
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void menuManageAccount(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Gestão de Conta-----\n\n", stdout);
		fputs("1) Mostrar saldo\n", stdout);
		fputs("2) Modificar saldo\n", stdout);
		fputs("3) Modificar dados da conta\n", stdout);
		fputs("4) Eliminar conta\n", stdout);
		fputs("5) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				showBalance();
				break;
			case '2': cleanScreen();
				changeBalance();
				break;
			case '3': cleanScreen();
				menuChangeUser();
				break;
			case '4': cleanScreen();
				deleteAccount();
				break;
			case '5': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void showBalance(){
	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;

	struct fileStructure file_userdb = openFile("userdb", "READ");
	if (!file_userdb.filePointer)
		return;


	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			cleanScreen();
			fprintf(stdout, "Saldo: %.2f €\n\n\n", userBalance);
			break;
		}
	}

	closeFile(file_userdb);
}

void changeBalance(){

	fputs("-----Mudar Saldo de Conta Bancária-----\n\n", stdout);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE];
	float fileBalance;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &fileBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	closeFile(file_userdb);


	char newStringBalance[MONEY_MAX_SIZE_STRING+2];

	fprintf(stdout, "\n\n\nSaldo atual: %.2f\n\n", fileBalance);
	fputs("Novo saldo (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ','): ", stdout);
	getString(newStringBalance, MONEY_MAX_SIZE_STRING+2);

	if (!checkNewBalance(newStringBalance)){
		cleanScreen();
		fputs("Erro! Novo saldo não válido.\n\n\n", stdout);
		return;
	}

	//transformar a string com o novo saldo para um float
	float newBalance = stringToFloat(newStringBalance);


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, newBalance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fputs("O saldo da sua conta bancária foi alterado com sucesso!\n\n\n", stdout);
}

void menuChangeUser(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='5'){
		fputs("-----Modificar Dados da Conta-----\n\n", stdout);
		fputs("1) Mudar nome\n", stdout);
		fputs("2) Mudar password\n", stdout);
		fputs("3) Mudar e-mail\n", stdout);
		fputs("4) Mudar contacto telefónico\n", stdout);
		fputs("5) Voltar ao menu de gestão de conta\n", stdout);
		printf("\nEscolha: ");
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				changeName();
				break;
			case '2': cleanScreen();
				changePass();
				break;
			case '3': cleanScreen();
				changeEmail();
				break;
			case '4': cleanScreen();
				changeContact();
				break;
			case '5': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void changeName(){

	fputs("-----Mudar Nome-----\n\n", stdout);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE+1], fileName[NAME_MAX_SIZE+1];

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%*[^;];%*[^;];%*[^;];%*f;%*[^;];%*s\n", fileName, fileUsername)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	closeFile(file_userdb);


	char newName[NAME_MAX_SIZE+2];

	fprintf(stdout, "\n\n\nNome atual: %s\n\n", fileName);
	fputs("Novo nome (não pode conter ';' e tem que ter entre 6 a 50 caracteres): ", stdout);
	getString(newName, NAME_MAX_SIZE+2);

	if (!checkNewName(newName)){
		cleanScreen();
		fputs("Erro! Novo nome não válido.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", newName, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fputs("O seu nome foi alterado com sucesso!\n\n\n", stdout);
}

void changePass(){

	fputs("-----Mudar Password-----\n\n", stdout);


	char newPass[PASSWORD_MAX_SIZE+2];

	//aqui não é dito a password antiga do utilizador para "ninguem" saber
	fputs("\n\nNova password (6 a 50 caracteres e não são permitidos ';' nem espaços): ", stdout);
	getString(newPass, PASSWORD_MAX_SIZE+2);

	if (!checkNewPass(newPass)){
		cleanScreen();
		fputs("Erro! Nova password não válida.\n\n\n", stdout);
		return;
	}


	struct fileStructure file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, newPass, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fputs("A sua password foi alterada com sucesso!\n\n\n", stdout);
}

void changeEmail(){

	fputs("-----Mudar E-Mail-----\n\n", stdout);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE], fileEmail[EMAIL_MAX_SIZE+1];

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%[^;];%*[^;];%*f;%*[^;];%*s\n", fileUsername, fileEmail)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	closeFile(file_userdb);


	char newEmail[EMAIL_MAX_SIZE+2];

	fprintf(stdout, "\n\n\nE-Mail atual: %s\n\n", fileEmail);
	fputs("* = obrigatório ser original\n", stdout);
	fputs("Novo e-mail (não pode conter ';' e ter no máximo 50 caracteres)*: ", stdout);
	getString(newEmail, EMAIL_MAX_SIZE+2);

	if (!checkNewEmail(newEmail)){
		cleanScreen();
		fputs("Erro! Novo e-mail não válido.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	//ver se já existe algum utilizador com o mesmo e-mail
	while (fscanf(file_userdb.filePointer, "%*[^;];%*[^;];%*[^;];%[^;];%*[^;];%*f;%*[^;];%*s\n", fileEmail)!=EOF){
		if ( !strcmp(fileEmail, newEmail)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o e-mail \"%s\" já existe.\n\n\n", newEmail);
			return;
		}
	}


	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				newEmail, tempData.contact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fputs("O seu e-mail foi alterado com sucesso!\n\n\n", stdout);
}

void changeContact(){

	fputs("-----Mudar Contacto Telefónico-----\n\n", stdout);


	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;

	char fileUsername[USERNAME_MAX_SIZE], fileContact[CONTACT_SIZE_2+1];

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%[^;];%*f;%*[^;];%*s\n", fileUsername, fileContact)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	closeFile(file_userdb);


	char newContact[CONTACT_SIZE_2+2];

	fprintf(stdout, "\n\n\nContacto telefónico atual: %s\n\n", fileContact);
	fputs("* = obrigatório ser original\n", stdout);
	fputs("Novo contacto telefónico (conter 9 ou 12 números)*: ", stdout);
	getString(newContact, CONTACT_SIZE_2+2);

	if (!checkNewContact(newContact)){
		cleanScreen();
		fputs("Erro! Novo contacto telefónico não válido.\n\n\n", stdout);
		return;
	}


	file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	//ver se já existe algum utilizador com o mesmo contacto
	while (fscanf(file_userdb.filePointer, "%*[^;];%*[^;];%*[^;];%*[^;];%[^;];%*f;%*[^;];%*s\n", fileContact)!=EOF){
		if ( !strcmp(fileContact, newContact)) {
			closeFile(file_userdb);
			cleanScreen();
			fprintf(stdout, "Erro! O utilizador com o contacto telefónico \"%s\" já existe.\n\n\n", newContact);
			return;
		}
	}

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, newContact, tempData.balance, tempData.perms, tempData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();
	fputs("O seu contacto telefónico foi alterado com sucesso!\n\n\n", stdout);
}

void deleteAccount(){

	char username[USERNAME_MAX_SIZE+2];

	fputs("-----Eliminar Conta-----\n\n", stdout);


	char confirmation[CONFIRMATION_MAX_SIZE+2];

	fputs("\nTem mesmo a certeza que pretende eliminar a sua conta? (SIM/NAO): ", stdout);
	getString(confirmation, CONFIRMATION_MAX_SIZE+2);

	if (!checkConfirmation(confirmation)){
		cleanScreen();
		fputs("Ação cancelada!\n\n\n", stdout);
		return;
	}


	struct fileStructure file_userdb = openFile("userdb", "WRITE");

	if (!file_userdb.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");

	if (!file_temp.filePointer)
		return;


	//estrutura para meter as informações do ficheiro
	struct userData tempData;

	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempData.name, tempData.username, 
			tempData.password, tempData.email, tempData.contact, &tempData.balance, tempData.perms, tempData.logged) != EOF){

		if ( !strcmp(tempData.username, loggedUsername))
			continue;      /*ao chegarmos ao utilizador que queremos eliminar, passar à frente a escrita 
							das informações desse utilizador no ficheiro temp*/
		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempData.name, tempData.username, tempData.password, 
				tempData.email, tempData.contact, tempData.balance, tempData.perms, tempData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);
	cleanScreen();


	//variaveis para concatenar
	char createDir[FILE_MAX_SIZE]="usersdb/";
	strcat(createDir, username);

	char createShoplist[FILE_MAX_SIZE]="usersdb/";
	strcat(createShoplist, username);
	strcat(createShoplist, "/");
	strcat(createShoplist, "shoplist");

	//remover os diretórios do utilizador a ser eliminado
	remove(createShoplist);
	remove(createDir);
	
	cleanScreen();
	fputs("A sua conta foi eliminada com sucesso!\n\n\n", stdout);
	fputs("Eliminou a sua conta. O programa irá agora terminar.\n", stdout);
	fputs("Saindo do programa...\n", stdout);
	exit(0);
}

void menuManageShoppingList(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='8'){
		fputs("-----Menu de compras-----\n\n", stdout);
		fputs("1) Pesquisar produtos\n", stdout);
		fputs("2) Comprar um produto individual\n", stdout);
		fputs("3) Ver o meu carrinho\n", stdout);
		fputs("4) Adicionar produtos ao carrinho\n", stdout);
		fputs("5) Mudar quantidade a comprar de um produto\n", stdout);
		fputs("6) Remover produtos do carrinho\n", stdout);
		fputs("7) Comprar produtos (Checkout)\n", stdout);
		fputs("8) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';

		switch(option[0]){
			case '1': cleanScreen();
				searchProduct();
				break;
			case '2': cleanScreen();
				buyIndividualProduct();
				break;
			case '3': cleanScreen();
				showCart();
				break;
			case '4': cleanScreen();
				addProductToCart();
				break;
			case '5': cleanScreen();
				changeCart();
				break;
			case '6': cleanScreen();
				deleteProductFromCart();
				break;
			case '7': cleanScreen();
				checkout();
				break;
			case '8': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void searchProduct(){
	char name[NAME_MAX_SIZE+2];

	fputs("-----Pesquisar Produtos-----\n\n\n", stdout);
	fputs("Padrão dos nomes dos produtos a pesquisar: (ENTER para listar todos os produtos) ", stdout);
	getString(name, NAME_MAX_SIZE+2);

	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	cleanScreen();      //limpar o ecra para ficar bonito

	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	int fileQuantity;
	float filePrice;

	int i, j;
	bool pattern, found_at_least_one=false;
	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName,
		&fileQuantity, &filePrice)!=EOF){
		
		for (i=0; fileName[i]!='\0'; i++){

			pattern=false;      //iniciar como falso
			if (fileName[i]==name[0]){      /*se a letra que estiver no index i da palavra fileName 
													for igual à primeira letra da palavra name*/
				pattern = true;
				for (j=1; fileName[i]!='\0' && name[j]!='\0'; j++){      /*como alguma letra do fileName é igual à 
																				primeira letra do name que queremos pesquisar, 
																				entrar noutro ciclo para percorrer o fileName 
																				e o name pesquisa ao mesmo tempo*/
					if (fileName[i+j]!=name[j]){
						pattern=false;
						break;
					}
				}
				if (fileName[i+j]=='\0' && name[j]!='\0')
					pattern=false;
				else
					pattern=true;
			}
			else if (name[0]=='\0'){
				pattern=true;
				i=0;      //i=0 e j=0 para não dar Segmentation Fault no teste de imprimir as coisas quando o name[0]=='\0'
				j=0;
			}
			else if (name[0]!='\0')
				pattern=false;

			if (pattern){
				found_at_least_one=true;
				fprintf(stdout, "Código: %s\n", fileCode);
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n\n\n", filePrice);
				break;
			}
		}
	}

	if ( !found_at_least_one)
		fprintf(stdout, "Não foi encontrado nenhum produto que tivesse \"%s\" contido no nome.\n\n\n", name);

	closeFile(file_productdb);
}

void buyIndividualProduct(){

	char code[CODE_SIZE+2];

	fputs("-----Comprar um produto-----\n\n", stdout);

	fputs("Código do produto: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe e se existir guardar quantidade existente e preço por unidade nas variáveis
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	bool found=false;
	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	int fileQuantity;
	float filePrice;

	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, &fileQuantity, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	//ver o dinheiro do utilizador
	struct fileStructure file_userdb = openFile("userdb", "READ");

	if (!file_userdb.filePointer)
		return;


	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	closeFile(file_userdb);


	char stringQuantity[QUANTITY_MAX_SIZE_STRING+2];

	fprintf(stdout, "\n\nSaldo da sua conta bancária: %.2f\n\n", userBalance);
	fprintf(stdout, "Nome do produto: %s\n", fileName);
	fprintf(stdout, "Preço por unidade: %.2f €\n\n", filePrice);
	fprintf(stdout, "Quantidade disponível: %d\n\n", fileQuantity);
	fputs("Quantidade (entre 0 e 2147483647 e tem que ser número inteiro): ", stdout);      //2147483647 = max int
	
	getString(stringQuantity, QUANTITY_MAX_SIZE_STRING+2);

	int quantity = stringToInt(stringQuantity);

	if (!checkNewQuantity(stringQuantity)){
		cleanScreen();
		fputs("Erro! Quantidade não válida.\n\n\n", stdout);
		return;
	}


	//alterar os ficheiros "userdb", "productdb" e acrescentar ao ficheiro "salesdb"
	file_userdb = openFile("userdb", "WRITE");
	if (!file_userdb.filePointer)
		return;

	file_productdb = openFile("productdb", "WRITE");
	if (!file_productdb.filePointer){
		closeFile(file_userdb);
		return;
	}

	struct fileStructure file_salesdb = openFile("salesdb", "APPEND");
	if (!file_salesdb.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		return;
	}


	//verificar se existe quantidade suficiente em stock
	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%d;%f\n", fileCode, &fileQuantity, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			if (fileQuantity - quantity < 0){
				closeFile(file_userdb);
				closeFile(file_productdb);
				closeFile(file_salesdb);
				cleanScreen();
				fputs("Erro! Quantidade não disponível em stock.\n\n\n", stdout);
				return;
			}
			break;
		}
	}

	//verificar se o utilizador tem dinheiro disponivel para comprar um produto
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			if (userBalance - filePrice*(float)quantity < 0){
				closeFile(file_userdb);
				closeFile(file_productdb);
				closeFile(file_salesdb);
				cleanScreen();
				fputs("Erro! Não tens dinheiro suficiente.\n\n\n", stdout);
				return;
			}
			break;
		}
	}


	//abrir o ficheiro "temp" para começar a editar os ficheiros
	struct fileStructure file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		return;
	}

	

	//estrutura para meter as informações do ficheiro "userdb"
	struct userData tempUserData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
			tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged) != EOF){

		if ( !strcmp(tempUserData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, userBalance-filePrice*(float)quantity, tempUserData.perms, tempUserData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);


	//abrir outra vez o ficheiro temp para editar o ficheiro "productdb"
	file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_productdb);
		closeFile(file_salesdb);
		return;
	}


	//estrutura para meter as informações do ficheiro
	struct productData tempProductData;

	fseek(file_productdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempProductData.code, tempProductData.name, 
			&tempProductData.quantity, &tempProductData.price) != EOF){

		if ( !strcmp(tempProductData.code, code))
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempProductData.code, tempProductData.name, 
				fileQuantity-quantity, tempProductData.price);

		else
			fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempProductData.code, tempProductData.name, 
				tempProductData.quantity, tempProductData.price);
	}


	rename("temp", "productdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_productdb);
	closeFile(file_temp);

	

	//escrever para o ficheiro "salesdb" a "fatura" da compra
	fprintf(file_salesdb.filePointer, "%s;%d;%s;%s/%s/%s\n", code, quantity, loggedUsername, realDay, realMonth, realYear);

	closeFile(file_salesdb);


	cleanScreen();
	fprintf(stdout, "Compraste %d unidades do produto com o código \"%s\" com sucesso!\n\n\n", quantity, code);
}

int showCart(){
	char fileName[NAME_MAX_SIZE+1], fileCode[CODE_SIZE+1], fileCode_2[CODE_SIZE+1];
	int fileWantedQuantity, fileAvailableQuantity;
	float filePrice;

	char shoplist[FILE_MAX_SIZE]="usersdb/";
	strcat(shoplist, loggedUsername);
	strcat(shoplist, "/");
	strcat(shoplist, "shoplist");

	struct fileStructure file_productdb = openFile("productdb", "READ");
	if (!file_productdb.filePointer){
		return 0;
	}

	struct fileStructure file_shoplist = openFile(shoplist, "READ");
	if (!file_shoplist.filePointer){
		closeFile(file_productdb);
		return 0;
	}


	int nr_of_products=0;
	bool found_at_least_one=false;
	cleanScreen();      //para ficar bonito o output
	fputs("Carrinho de compras:\n\n\n", stdout);
	while(fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity) != EOF){      //percorrer a lista de compras
		
		//para cada produto na lista de compras, ver as informações desse produto no ficheiro "productdb"
		fseek(file_productdb.filePointer, 0, SEEK_SET);
		while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode_2, fileName, &fileAvailableQuantity, &filePrice)!=EOF) {
			if (!strcmp(fileCode, fileCode_2)){
				found_at_least_one=true;
				nr_of_products++;
				fprintf(stdout, "Código: %s\n", fileCode);
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade a comprar: %d\n", fileWantedQuantity);
				fprintf(stdout, "Quantidade disponível: %d\n", fileAvailableQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n\n\n", filePrice);
			}
		}
	}


	if ( !found_at_least_one)
		fputs("Carrinho vazio.\n\n\n", stdout);

	closeFile(file_shoplist);
	closeFile(file_productdb);
	return nr_of_products;
}

void addProductToCart(){

	char code[CODE_SIZE+2];

	fputs("-----Adicionar um produto ao carrinho-----\n\n", stdout);

	fputs("Código do produto: ", stdout);
	getString(code, CODE_SIZE+2);
	
	//ver se o produto existe e se existir guardar quantidade existente e preço por unidade nas variáveis
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	bool found=false;
	char fileCode[CODE_SIZE+1], fileName[NAME_MAX_SIZE+1];
	int fileAvailableQuantity;
	float filePrice;

	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, &fileAvailableQuantity, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);

	//ver se já existe esse produto no carrinho
	char shoplist[FILE_MAX_SIZE]="usersdb/";      //8 ("userdb\") + 20 (username) + 8 (shoplist) + 1 ('\0')
	strcat(shoplist, loggedUsername);
	strcat(shoplist, "/");
	strcat(shoplist, "shoplist");

	struct fileStructure file_shoplist = openFile(shoplist, "READ");

	while (fscanf(file_shoplist.filePointer, "%[^;];%*d\n", fileCode)!=EOF){
		if ( !strcmp(fileCode, code)) {
			closeFile(file_shoplist);
			cleanScreen();
			fprintf(stdout, "Erro! O produto com o código \"%s\" já existe no seu carrinho. " 
				"Para alterar a quantidade a comprar por-favor use a opção de mudar quantidade no carrinho.\n", code);
			return;
		}
	}

	closeFile(file_shoplist);


	//ver o saldo do utilizador
	struct fileStructure file_userdb = openFile("userdb", "READ");

	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername))
			break;
	}

	closeFile(file_userdb);



	char stringQuantity[QUANTITY_MAX_SIZE_STRING+2];

	cleanScreen();
	fprintf(stdout, "\n\nSaldo da sua conta bancária: %.2f\n\n", userBalance);
	fprintf(stdout, "Preço por unidade: %.2f €\n\n", filePrice);
	fprintf(stdout, "Quantidade disponível: %d\n\n", fileAvailableQuantity);
	fputs("Quantidade (entre 0 e 2147483647 e tem que ser número inteiro): ", stdout);      //2147483647 = max int
	getString(stringQuantity, QUANTITY_MAX_SIZE_STRING+2);


	int quantity = stringToInt(stringQuantity);

	if (!checkNewQuantity(stringQuantity)){
		cleanScreen();
		fputs("Erro! Quantidade não válida.\n\n\n", stdout);
		return;
	}


	//shoplist="usersdb/%loggedUsername%/shoplist"
	file_shoplist = openFile(shoplist, "APPEND");

	if (!file_shoplist.filePointer)
		return;

	fprintf(file_shoplist.filePointer, "%s;%d\n", code, quantity);

	closeFile(file_shoplist);


	cleanScreen();
	fprintf(stdout, "O produto com o código \"%s\" foi adicionado ao seu carrinho com %d unidade%scom sucesso!\n\n\n", code, quantity,(quantity>1) ? "s " : " " );
}

void changeCart(){
	char code[CODE_SIZE+2];
	
	fputs("-----Mudar quantidade a comprar de um produto-----\n\n", stdout);

	fputs("Código do produto: ", stdout);
	getString(code, CODE_SIZE+2);


	char shoplist[FILE_MAX_SIZE]="usersdb/";
	strcat(shoplist, loggedUsername);
	strcat(shoplist, "/");
	strcat(shoplist, "shoplist");

	struct fileStructure file_shoplist = openFile(shoplist, "READ");

	char fileCode[CODE_SIZE+1];
	int fileWantedQuantity;
	bool found=false;

	while (fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
		}
	}

	if ( !found){
		closeFile(file_shoplist);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_shoplist);


	//ver se o produto ainda existe e se existir guardar quantidade existente e preço por unidade nas variáveis
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;


	found=false;
	char fileName[NAME_MAX_SIZE+1];
	int fileAvailableQuantity;
	float filePrice;

	while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, &fileAvailableQuantity, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado no seu carrinho.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	//ver o saldo do utilizador
	struct fileStructure file_userdb = openFile("userdb", "READ");

	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername))
			break;
	}

	closeFile(file_userdb);


	char newStringQuantity[QUANTITY_MAX_SIZE_STRING+1];

	fprintf(stdout, "\n\nSaldo: %.2f €\n\n", userBalance);
	fprintf(stdout, "Preço por unidade: %.2f €\n\n", filePrice);
	fprintf(stdout, "Quantidade disponível: %d\n\n", fileAvailableQuantity);
	fprintf(stdout, "Quantidade atual a comprar: %d\n\n\n", fileWantedQuantity);
	fputs("Nova quantidade (entre 0 e 2147483647 e tem que ser número inteiro): ", stdout);
	
	getString(newStringQuantity, QUANTITY_MAX_SIZE_STRING+2);

	int newQuantity = stringToInt(newStringQuantity);

	if (!checkNewQuantity(newStringQuantity)){
		cleanScreen();
		fputs("Erro! Nova quantidade não válida.\n\n\n", stdout);
		return;
	}
	else if (newQuantity==0){
		cleanScreen();
		fputs("Erro! Se pretender eliminar um produto do seu carrinho por-favor use a opção de "
			"eliminar um produto do carrinho", stdout);
		return;
	}


	file_productdb = openFile("productdb", "READ");
	if (!file_productdb.filePointer)
		return;


	//verificar se existe quantidade suficiente em stock
	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%d;%*f\n", fileCode, &fileAvailableQuantity)!=EOF){
		if ( !strcmp(fileCode, code)) {
			if (fileAvailableQuantity - newQuantity < 0){
				closeFile(file_productdb);
				cleanScreen();
				fputs("Erro! Quantidade não disponível em stock.\n\n\n", stdout);
				return;
			}
			break;
		}
	}

	closeFile(file_productdb);



	file_shoplist = openFile(shoplist, "WRITE");

	if (!file_shoplist.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_shoplist);
		return;
	}


	//estrutura para meter as informações do ficheiro "shoplist"
	struct shoplistData tempShoplistData;

	while (fscanf(file_shoplist.filePointer, "%[^;];%d\n", tempShoplistData.code, &tempShoplistData.quantity)!=EOF){

		if ( !strcmp(tempShoplistData.code, code))
			fprintf(file_temp.filePointer, "%s;%d\n", tempShoplistData.code, newQuantity);

		else
			fprintf(file_temp.filePointer, "%s;%d\n", tempShoplistData.code, tempShoplistData.quantity);
	}


	rename("temp", shoplist);      //substituir o antigo ficheiro "shoplist" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);


	cleanScreen();
	fprintf(stdout, "Foi alterada a quantidade a comprar do produto com o código \"%s\" com sucesso!\n\n\n", code);
}

void deleteProductFromCart(){
	char code[CODE_SIZE+2];
	
	fputs("-----Eliminar um produto do carrinho-----\n\n", stdout);

	fputs("Código do produto: ", stdout);
	getString(code, CODE_SIZE+2);


	char shoplist[FILE_MAX_SIZE]="usersdb/";
	strcat(shoplist, loggedUsername);
	strcat(shoplist, "/");
	strcat(shoplist, "shoplist");
	
	struct fileStructure file_shoplist = openFile(shoplist, "READ");

	char fileCode[CODE_SIZE+1];
	bool found=false;

	while (fscanf(file_shoplist.filePointer, "%[^;];%*d\n", fileCode)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
		}
	}

	if ( !found){
		closeFile(file_shoplist);
		cleanScreen();
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_shoplist);


	char confirmation[CONFIRMATION_MAX_SIZE+2];

	fprintf(stdout, "\nTem mesmo a certeza que pretende eliminar o produto com o código \"%s\" do seu carrinho? (SIM/NAO): ", code);
	getString(confirmation, CONFIRMATION_MAX_SIZE+2);

	if (!checkConfirmation(confirmation)){
		cleanScreen();
		fputs("Ação cancelada!\n\n\n", stdout);
		return;
	}



	file_shoplist = openFile(shoplist, "WRITE");

	if (!file_shoplist.filePointer)
		return;

	struct fileStructure file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_shoplist);
		return;
	}


	//estrutura para meter as informações do ficheiro "userdb"
	struct shoplistData tempShoplistData;

	while (fscanf(file_shoplist.filePointer, "%[^;];%d\n", tempShoplistData.code, &tempShoplistData.quantity)!=EOF){

		if ( !strcmp(tempShoplistData.code, code))
			continue;

		else
			fprintf(file_temp.filePointer, "%s;%d\n", tempShoplistData.code, tempShoplistData.quantity);
	}


	rename("temp", shoplist);      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_shoplist);
	closeFile(file_temp);

	cleanScreen();
	fprintf(stdout, "O produto com o código \"%s\" foi eliminado do ser carrinho com sucesso!\n\n\n", code);
}

void checkout(){
	
	fputs("-----Checkout-----\n\n", stdout);

	char confirmation[CONFIRMATION_MAX_SIZE+2];

	if (showCart()==0){      //mostrar o carrinho de compras de utilizador
		fputs("Erro! Não pode fazer checkout com o carrinho vazio.", stdout);
		return;
	}

	//mostrar o saldo do utilizador
	struct fileStructure file_userdb = openFile("userdb", "READ");

	char fileUsername[USERNAME_MAX_SIZE+1];
	float userBalance;

	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			cleanScreen();
			fprintf(stdout, "\n\nSaldo: %.2f €\n\n", userBalance);
			break;
		}
	}

	closeFile(file_userdb);


	//ver o preço total
	struct fileStructure file_productdb = openFile("productdb", "READ");
	if (!file_productdb.filePointer)
		return;
	
	char shoplist[FILE_MAX_SIZE]="usersdb/";
	strcat(shoplist, loggedUsername);
	strcat(shoplist, "/");
	strcat(shoplist, "shoplist");

	struct fileStructure file_shoplist = openFile(shoplist, "READ");
	if (!file_shoplist.filePointer){
		closeFile(file_productdb);
		return;
	}

	char fileCode[CODE_SIZE+1], fileCode_2[CODE_SIZE+1];
	int fileWantedQuantity;
	float filePrice, totalPrice=0;

	while(fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity) != EOF){      //percorrer a lista de compras
		
		fseek(file_productdb.filePointer, 0, SEEK_SET);
		while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode_2, &filePrice)!=EOF) {
			if ( !strcmp(fileCode, fileCode_2))
				totalPrice+=(float)fileWantedQuantity*filePrice;
		}
	}

	fprintf(stdout, "Preço total: %.2f €\n\n", totalPrice);

	closeFile(file_productdb);
	closeFile(file_shoplist);



	fputs("Tem mesmo a certeza que deseja comprar todos os produtos e "
		"respetivas quantidades? (SIM/NÃO): ", stdout);
	getString(confirmation, CONFIRMATION_MAX_SIZE+2);

	if (!checkConfirmation(confirmation)){
		cleanScreen();
		fputs("Ação cancelada!\n\n\n", stdout);
		return;
	}



	file_userdb = openFile("userdb", "WRITE");
	if (!file_userdb.filePointer)
		return;

	file_productdb = openFile("productdb", "WRITE");
	if (!file_productdb.filePointer){
		closeFile(file_userdb);
		return;
	}

	struct fileStructure file_salesdb = openFile("salesdb", "APPEND");
	if (!file_salesdb.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		return;
	}

	file_shoplist = openFile(shoplist, "WRITE");
	if (!file_shoplist.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		return;
	}


	bool good_to_go=true;
	int fileAvailableQuantity;
	totalPrice=0;

	while(fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity) != EOF){      //percorrer a lista de compras
		fseek(file_productdb.filePointer, 0, SEEK_SET);
		while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%d;%f\n", fileCode_2, &fileAvailableQuantity, &filePrice)!=EOF) {
			if ( !strcmp(fileCode, fileCode_2)){

				if (fileWantedQuantity > fileAvailableQuantity){
					fprintf(stdout, "\n\nNão existe quantidade suficiente no armazem do produto com o código \"%s\"", fileCode);
					good_to_go=false;
					break;
				}
				else {
					totalPrice+=(float)fileWantedQuantity*filePrice;
					break;
				}
			}
		}
	}

	if ( !good_to_go) {
		fputs("\n\nPor-favor corrija a quantidade de produtos a comprar e volte a fazer checkout.\n\n\n", stdout);
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		closeFile(file_shoplist);
		return;
	}

	
	//ver se o utilizador tem dinheiro suficiente para todos os produtos
	while (fscanf(file_userdb.filePointer, "%*[^;];%[^;];%*[^;];%*[^;];%*[^;];%f;%*[^;];%*s\n", fileUsername, &userBalance)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername)) {
			break;
		}
	}

	if (userBalance - totalPrice < 0 || totalPrice < 0){      //totalPrice < 0 é para caso haja overflow da variável totalPrice
		fputs("\n\n\nErro! Saldo insuficiente. Por-favor altere o seu carrinho.\n\n\n", stdout);
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		closeFile(file_shoplist);
		return;
	}


	//como tudo está em ordem, podemos começar a editar os ficheiros
	//abrir o ficheiro "temp" para começar a editar os ficheiros
	//alterar o ficheiro "userdb"
	struct fileStructure file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		closeFile(file_shoplist);
		return;
	}

	

	//estrutura para meter as informações do ficheiro "userdb"
	struct userData tempUserData;

	fseek(file_userdb.filePointer, 0, SEEK_SET);
	while(fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
			tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged) != EOF){

		if ( !strcmp(tempUserData.username, loggedUsername))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, userBalance-totalPrice, tempUserData.perms, tempUserData.logged);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}


	rename("temp", "userdb");      //substituir o antigo ficheiro "userdb" pelo novo
	closeFile(file_userdb);
	closeFile(file_temp);


	//alterar o ficheiro "productdb"
	file_temp = openFile("temp", "APPEND");
	
	if (!file_temp.filePointer){
		closeFile(file_userdb);
		closeFile(file_productdb);
		closeFile(file_salesdb);
		closeFile(file_shoplist);
		return;
	}


	struct productData tempProductData;

	fseek(file_shoplist.filePointer, 0, SEEK_SET);
	while(fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity) != EOF){      //percorrer a lista de compras
		
		fseek(file_productdb.filePointer, 0, SEEK_SET);
		while (fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", tempProductData.code, tempProductData.name, 
			&tempProductData.quantity, &tempProductData.price) != EOF){

			if ( !strcmp(fileCode, tempProductData.code))
				fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempProductData.code, tempProductData.name, 
					tempProductData.quantity-fileWantedQuantity, tempProductData.price);

			else
				fprintf(file_temp.filePointer, "%s;%s;%d;%f\n", tempProductData.code, tempProductData.name, 
					tempProductData.quantity, tempProductData.price);
		}
	}

	rename("temp", "productdb");      //substituir o antigo ficheiro "product" pelo novo
	closeFile(file_productdb);
	closeFile(file_temp);


	//meter a lista de compras no ficheiro "salesdb"
	fseek(file_shoplist.filePointer, 0, SEEK_SET);
	while(fscanf(file_shoplist.filePointer, "%[^;];%d\n", fileCode, &fileWantedQuantity) != EOF){
		fprintf(file_salesdb.filePointer, "%s;%d;%s;%s/%s/%s\n", fileCode, fileWantedQuantity, 
			loggedUsername, realDay, realMonth, realYear);
	}

	closeFile(file_salesdb);

	if ( ftruncate(file_shoplist.fd, 0) != 0)      //eliminar tudo o que está no ficheiro "shoplist"
		fputs("Erro ao limpar ficheiro \"shoplist\"!\n\n\n", stderr);

	closeFile(file_shoplist);

	cleanScreen();
	fputs("Foram comprados todos os produtos do seu carrinho com sucesso.\n\n\n", stdout);
}

void menuStats(){

	char option[OPTION_MAX_SIZE+2]="0";

	while(option[0]!='7'){
		fputs("-----Menu de Estatísticas de Compras-----\n\n", stdout);
		fputs("1) Estatísticas de compras de um produto\n", stdout);
		fputs("2) TOP de produtos comprados\n", stdout);
		fputs("3) Estatísticas diárias\n", stdout);
		fputs("4) TOP produtos comprados num dia\n", stdout);
		fputs("5) Estatísticas mensais\n", stdout);
		fputs("6) TOP produtos comprados num mes\n", stdout);
		fputs("7) Voltar ao menu principal\n", stdout);
		fputs("\nEscolha: ", stdout);
		getString(option, OPTION_MAX_SIZE+2);

		if ( !checkNewOption(option))
			option[0]='a';      /*como todas as opções sao algarismos, isto vai fazer 
								com que a opção não seja reconhecida*/

		switch(option[0]){
			case '1': cleanScreen();
				productStats();
				break;
			case '2': cleanScreen();
				topStats();
				break;
			case '3': cleanScreen();
				dailyStats();
				break;
			case '4': cleanScreen();
				topDailyStats();
				break;
			case '5': cleanScreen();
				monthlyStats();
				break;
			case '6': cleanScreen();
				topMonthlyStats();
				break;
			case '7': cleanScreen();
				break;
			default: cleanScreen();
				fputs("Erro! Opção não reconhecida.\n\n\n", stdout);
				break;
		}
	}
}

void productStats(){
	char code[CODE_SIZE+2];

	fputs("-----Estatísticas de Compras de um Produto-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	//meter as informações para uma estrutura
	struct salesData tempData;
	long int nr_of_unit_sold = 0;

	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%*[^/]/%*[^/]/%*s\n", tempData.code, 
		&tempData.quantity, tempData.username)!=EOF){
		
		if (!strcmp(code, tempData.code) && !strcmp(loggedUsername, tempData.username))
			nr_of_unit_sold += tempData.quantity;
	}

	closeFile(file_salesdb);


	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Número de unidades compradas do produto com o código \"%s\": %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Despesa: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}

void topStats(){
	fputs("-----Estatísticas de Compras de Produtos (TOP)-----\n\n\n", stdout);


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileUsername[USERNAME_MAX_SIZE+1];
	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%[^;];%*[^/]/%*[^/]/%*s\n", fileUsername)!=EOF){
		if ( !strcmp(fileUsername, loggedUsername))
			linhas++;
	}

	if ( !linhas){
		closeFile(file_salesdb);
		fputs("Erro! Ainda não compraste nada.\n\n\n", stdout);
		return;
	}

	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%*[^/]/%*[^/]/%*s\n", fileCode, &fileQuantity, fileUsername)!=EOF){

		if ( !strcmp(fileUsername, loggedUsername)){
			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades compradas: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Despesa: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
		else
			fprintf(stdout, "\n\n");
	}


	closeFile(file_productdb);

}

void dailyStats(){
	char code[CODE_SIZE+2], day[DAY_MAX_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas diárias-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	fputs("\n\nData para processamento de estatísticas:\n\n", stdout);
	fputs("Dia: ", stdout);
	getString(day, DAY_MAX_SIZE+2);
	if (!checkDay(day)){
		cleanScreen();
		fprintf(stdout, "Erro! Dia não válido.\n\n\n");
		return;
	}

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileDay[DAY_MAX_SIZE+1], fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	char fileUsername[USERNAME_MAX_SIZE+1];
	int fileQuantity;
	long int nr_of_unit_sold = 0;
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%[^/]/%[^/]/%s\n", fileCode, &fileQuantity, fileUsername, 
		fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(code, fileCode) && !strcmp(day, fileDay) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear) && !strcmp(loggedUsername, fileUsername))

			nr_of_unit_sold += fileQuantity;
	}

	closeFile(file_salesdb);

	if ( !nr_of_unit_sold){
		cleanScreen();
		fprintf(stdout, "Erro! Não compraste nenhuma unidade do produto com o código \"%s\" na data referida.\n\n\n", code);
		return;
	}

	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Data de estatísticas: %s/%s/%s \n\n", day, month, year);
	fprintf(stdout, "Número de unidades compradas do produto com o código \"%s\" na data referida: %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Despesa: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}

void topDailyStats(){
	char day[DAY_MAX_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas diárias (TOP)-----\n\n\n", stdout);
	fputs("Data para processamento de estatística\n\n", stdout);

	fputs("Dia: ", stdout);
	getString(day, DAY_MAX_SIZE+2);
	if (!checkDay(day)){
		cleanScreen();
		fprintf(stdout, "Erro! Dia não válido.\n\n\n");
		return;
	}

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileUsername[USERNAME_MAX_SIZE+1], fileDay[DAY_MAX_SIZE+1], fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%[^;];%[^/]/%[^/]/%s\n", fileUsername, fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(day, fileDay) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear) && !strcmp(loggedUsername, fileUsername))

			linhas++;
	}

	if ( !linhas){
		cleanScreen();
		closeFile(file_salesdb);
		fputs("Erro! Não compraste nenhum produto na data referida.\n\n\n", stdout);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%[^/]/%[^/]/%s\n", fileCode, &fileQuantity, fileUsername, 
			fileDay, fileMonth, fileYear)!=EOF){

		if (!strcmp(fileDay, day) && !strcmp(fileMonth, month) && 
			!strcmp(fileYear, year) && !strcmp(loggedUsername, fileUsername)) {
			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();
	fprintf(stdout, "Data de estatísticas: %s/%s/%s \n\n", day, month, year);

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades compradas na data referida: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Despesa: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
		else
			fprintf(stdout, "\n\n");
	}


	closeFile(file_productdb);
}

void monthlyStats(){
	char code[CODE_SIZE+2], month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas Mensais-----\n\n\n", stdout);
	fputs("Código do produto a processar estatísticas: ", stdout);
	getString(code, CODE_SIZE+2);


	//ver se o produto existe no stock
	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;

	char fileCode[CODE_SIZE+1];
	float filePrice;
	bool found=false;

	while (fscanf(file_productdb.filePointer, "%[^;];%*[^;];%*d;%f\n", fileCode, &filePrice)!=EOF){
		if ( !strcmp(fileCode, code)) {
			found=true;
			break;
		}
	}

	if ( !found){
		closeFile(file_productdb);
		fprintf(stdout, "Erro! O produto com o código \"%s\" não foi encontrado.\n\n\n", code);
		return;
	}

	closeFile(file_productdb);


	fputs("\n\nData para processamento de estatísticas:\n\n", stdout);
	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	char fileUsername[USERNAME_MAX_SIZE+1];
	int fileQuantity;
	long int nr_of_unit_sold = 0;
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%*[^/]/%[^/]/%s\n", fileCode, &fileQuantity, fileUsername, 
		fileMonth, fileYear)!=EOF){

		if (!strcmp(code, fileCode) && !strcmp(month, fileMonth) && 
			!strcmp(year, fileYear) && !strcmp(loggedUsername, fileUsername))

			nr_of_unit_sold += fileQuantity;
	}

	closeFile(file_salesdb);

	if ( !nr_of_unit_sold){
		cleanScreen();
		fputs("Erro! Não compraste nenhum produto na data referida.\n\n\n", stdout);
		return;
	}


	cleanScreen();      //limpar o ecra para ficar bonito
	fprintf(stdout, "Data de estatísticas: %s/%s \n\n", month, year);
	fprintf(stdout, "Número de unidades compradas do produto com o código \"%s\" na data referida: %ld\n", code, nr_of_unit_sold);
	fprintf(stdout, "Despesa: %.2lf €\n\n\n", filePrice*(float)nr_of_unit_sold);
}


void topMonthlyStats(){

	char month[MONTH_MAX_SIZE+2], year[YEAR_MAX_SIZE+2];

	fputs("-----Estatísticas Mensais (TOP)-----\n\n\n", stdout);
	fputs("Data para processamento de estatística\n\n", stdout);

	fputs("Mes: ", stdout);
	getString(month, MONTH_MAX_SIZE+2);
	if (!checkMonth(month)){
		cleanScreen();
		fprintf(stdout, "Erro! Mes não válido.\n\n\n");
		return;
	}

	fputs("Ano: ", stdout);
	getString(year, YEAR_MAX_SIZE+2);      //6 para ex:2017 + verificação de erro + '\0'
	if (!checkYear(year)){
		cleanScreen();
		fprintf(stdout, "Erro! Ano não válido.\n\n\n");
		return;
	}


	struct fileStructure file_salesdb = openFile("salesdb", "READ");

	if (!file_salesdb.filePointer)
		return;


	char fileUsername[USERNAME_MAX_SIZE+1], fileMonth[MONTH_MAX_SIZE+1], fileYear[YEAR_MAX_SIZE+1];
	int linhas=0;      //número máximo de posições ocupadas (número máximo de produtos diferentes vendidos na data referida)
	while (fscanf(file_salesdb.filePointer, "%*[^;];%*d;%[^;];%*[^/]/%[^/]/%s\n", fileUsername, fileMonth, fileYear)!=EOF){

		if (!strcmp(month, fileMonth) && !strcmp(year, fileYear) && 
			!strcmp(loggedUsername, fileUsername))

			linhas++;
	}

	if ( !linhas){
		cleanScreen();
		closeFile(file_salesdb);
		fputs("Erro! Não compraste nenhum produto na data referida.\n\n\n", stdout);
		return;
	}


	struct salesData tempData[linhas];

	//voltar com o pointer para o início do ficheiro para usar no fscanf
	fseek(file_salesdb.filePointer, 0, SEEK_SET);

	int linhas2=0, i;      //linhas2 = número de produtos diferentes vendidos naquele dia
	bool found=false;
	char fileCode[CODE_SIZE+1];
	int fileQuantity;
	//meter as informações do ficheiro dentro do array de estruturas
	while (fscanf(file_salesdb.filePointer, "%[^;];%d;%[^;];%*[^/]/%[^/]/%s\n", fileCode, &fileQuantity, fileUsername, 
			fileMonth, fileYear)!=EOF){

		if (!strcmp(fileMonth, month) && !strcmp(fileYear, year) && 
			!strcmp(loggedUsername, fileUsername)) {

			found=false;
			for (i=0; i<linhas2; i++){
				if (!strcmp(fileCode, tempData[i].code)){
					found=true;
					break;
				}
			}
			if (found)
				tempData[i].quantity+=fileQuantity;
			else {
				strcpy(tempData[linhas2].code, fileCode);
				tempData[linhas2].quantity = fileQuantity;
				linhas2++;
			}
		}
	}

	//se metermos closeFile(file_salesdb); dá segfault
	file_salesdb.lock->l_type = F_UNLCK;

	fcntl(file_salesdb.fd, F_SETLKW, file_salesdb.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (file_salesdb.filePointer)
		fclose(file_salesdb.filePointer);

	close(file_salesdb.fd);

	//ordenar por quantidades vendidas
	struct salesData tempData_2;
	for (i=0; i<linhas2; i++){
		for (int j=i; j<linhas2; j++){
			if (tempData[j].quantity > tempData[i].quantity){
				tempData_2=tempData[i];
				tempData[i]=tempData[j];
				tempData[j]=tempData_2;
			}
		}
	}


	struct fileStructure file_productdb = openFile("productdb", "READ");

	if (!file_productdb.filePointer)
		return;
	
	char fileName[NAME_MAX_SIZE+1];
	float filePrice;
	found=false;

	cleanScreen();
	fprintf(stdout, "Data de estatísticas: %s/%s \n\n", month, year);

	for (i=0; i<linhas2; i++){

		fprintf(stdout, "#%d\n", i+1);
		fprintf(stdout, "Código: %s\n", tempData[i].code);      //imprimir código
		fseek(file_productdb.filePointer, 0, SEEK_SET);      //andar com o ponteiro do ficheiro para o inicio do ficheiro
		found=false;

		while(fscanf(file_productdb.filePointer, "%[^;];%[^;];%d;%f\n", fileCode, fileName, 
			&fileQuantity, &filePrice)!=EOF){      //ir buscar informações do produto à base de dados de produtos

			if (!strcmp(tempData[i].code, fileCode)){
				fprintf(stdout, "Nome: %s\n", fileName);
				fprintf(stdout, "Quantidade disponível: %d\n", fileQuantity);
				fprintf(stdout, "Preço por unidade: %.2f €\n", filePrice);
				found=true;
				break;
			}
		}
		if (!found)
			fputs("Produto não existente na base de dados.\n", stdout);

		fprintf(stdout, "Unidades compradas na data referida: %d\n", tempData[i].quantity);

		if (found)
			fprintf(stdout, "Lucro: %.2lf €\n\n\n", filePrice*(float)tempData[i].quantity);
		else
			fprintf(stdout, "\n\n");
	}


	closeFile(file_productdb);
}

bool checkNewOption(char option[]){
	if (strlen(option)>1)
		return false;

	return true;
}

bool checkNewName(char name[]){
	//Nome completo (não pode conter ';' e tem que ter entre 6 a 50 letras)
	if (strlen(name)<NAME_MIN_SIZE || strlen(name)>NAME_MAX_SIZE)
		return false;

	for (int i=0; name[i]!='\0'; i++)
		if (name[i]==';')
			return false;

	return true;
}

bool checkNewUsername(char username[]){
	//Username (6 a 20 caracteres e não são permitidos ';' nem espaços)
	if (strlen(username)<USERNAME_MIN_SIZE || strlen(username)>USERNAME_MAX_SIZE)
		return false;

	for (int i=0; username[i]!='\0'; i++)
		if (username[i]==';' || username[i]==' ')
			return false;

	return true;
}

bool checkNewPass(char password[]){
	//Password (6 a 20 caracteres e não são permitidos ';' nem espaços)
	if (strlen(password)<PASSWORD_MIN_SIZE || strlen(password)>PASSWORD_MAX_SIZE)
		return false;

	for (int i=0; password[i]!='\0'; i++)
		if (password[i]==';' || password[i]==' ')
			return false;

	return true;
}

bool checkNewEmail(char email[]){
	//Endereço de e-mail: (não pode conter ';' e tem que ter entre 6 (a@a.pt) a 50 letras)
	if (strlen(email)<6 || strlen(email)>EMAIL_MAX_SIZE)
		return false;

	int i=0;
	int domain_cnt=0;
	int domain_cnt2=0;
	int domain_start;

	for (; email[i]!='@' && email[i]!='\0'; i++){      //ir até ao @ ou ao fim se não existir @
		if (email[i]==';')
			return false;
	}

	if (i==0)      //falso se não existir nada antes do @ ou não existir @
		return false;

	for (; email[i]!='.'; i++){      //ir até ao 1º ponto
		if (email[i]==';')
			return false;
		domain_cnt++;
	}

	if (domain_cnt==0)      //falso se o nome de domínio antes do .com.pt, .com ou .pt for vazio
		return false;

	for (; email[i]!='\0'; i++){      //ir até ao fim
		if (email[i]==';')
			return false;
		domain_cnt2++;
	}

	domain_start=i-domain_cnt2+1;      //i - domain_cnt2 = posição do 1º ponto   +1 = posição a seguir ao 1º ponto
	if (domain_cnt2==3){
		if (email[domain_start]=='p' && email[domain_start+1]=='t' && email[domain_start+2]=='\0')      //se for do tipo .pt (3 caracteres)
			return true;
	}

	else if (domain_cnt2==4){
		if (email[domain_start]=='c' && email[domain_start+1]=='o' && email[domain_start+2]=='m' && email[domain_start+3]=='\0')      //se for do tipo .com (4 caracteres)
			return true;
	}

	else if (domain_cnt2==7){
		if (email[domain_start]=='c' && email[domain_start+1]=='o' && email[domain_start+2]=='m' && email[domain_start+3]=='.'
			&& email[domain_start+4]=='p' && email[domain_start+5]=='t' && email[domain_start+6]=='\0')      //se for do tipo .com.pt (7 caracteres)
			return true;
	}


	return false;
}

bool checkNewContact(char contact[]){
	//Contacto telefónico (conter 9 (número local) ou 12 números (número local + código de pais)):
	if (strlen(contact)!=CONTACT_SIZE_1 && strlen(contact)!=CONTACT_SIZE_2)
		return false;

	for (int i=0; contact[i]!='\0'; i++)
		if (contact[i]<'0' && contact[i]>'9')
			return false;

	return true;
}

bool checkNewBalance(char stringBalance[]){
	//Montande inicial da conta bancária (entre 0.0 e 340282346638528859811704183484516925440.0, até 2 casas decimais e usar '.' em vez de ',')

	//se o dinheiro tiver mais do que 42 caracteres (max int float + . + 2 casas decimais)
	if (strlen(stringBalance)>MONEY_MAX_SIZE_STRING)
		return false;

	int dot_cnt=0, integer=0, decimal=0;
	for (int i=0; stringBalance[i]!='\0'; i++){
		if (stringBalance[i]<'0' || stringBalance[i]>'9'){
			if (stringBalance[i]=='.')
				dot_cnt++;      //se apanhar um ponto ou vírgula, aumentar contador de pontos
			else
				return false;
		}
		
		if (dot_cnt>1)      //se tiver mais do que um ponto ou vírgula
			return false;
		else if (dot_cnt==0)
			integer++;
		else if (dot_cnt==1)
			decimal++;
		else if (integer>39)      //se tiver mais do que 39 caracteres antes do ponto ou vírgula
			return false;	
		else if (decimal>2){      //se tiver mais do que duas casas decimais
			return false;
		}
	}

	float balance = stringToFloat(stringBalance);

	//340282346638528859811704183484516925440.0 = max float
	//balance<0 para ver se excedeu o limite de float
	if (balance>MONEY_MAX_SIZE_FLOAT || balance<0)
		return false;

	//se chegou a este ponto é porque o saldo está certo
	//se não existirem duas casas decimais, meter-las
	if (dot_cnt==0){
		int len = (int)strlen(stringBalance);
		stringBalance[len]='.';
		stringBalance[len+1]='0';
		stringBalance[len+2]='0';
		stringBalance[len+3]='\0';
	}
	else if (dot_cnt==1){

		if (decimal==0){
			int len = (int)strlen(stringBalance);
			stringBalance[len]='0';
			stringBalance[len+1]='0';
			stringBalance[len+2]='\0';
		}
		else if (decimal==1){
			int len = (int)strlen(stringBalance);
			stringBalance[len]='0';
			stringBalance[len+1]='\0';
		}
	}

	return true;
}

bool checkNewPerms(char perms[]){
	//Permissões -> 1 = Administrador, 2 = Utilizador
	if (strlen(perms)!=PERMS_MAX_SIZE)
		return false;

	if (perms[0]!='1' && perms[0]!='2')
		return false;

	return true;
}

bool checkNewCode(char code[]){
	if (strlen(code)==CODE_SIZE){      //se o código tiver 9 caracteres, verificar se são todos algarismos
		for (int i=0; i<9; i++){
			if (code[i]<'0' || code[i]>'9')
				return false;
		}
		return true;      //caso o código tenha 9 algarismos e não exista nenhum caracter sem ser algarismo
	}
	else if (strlen(code)==0)      //se o código for vazio (usar código aleatório)
		return true;

	return false;
}

bool checkNewQuantity(char stringQuantity[]){
	if (strlen(stringQuantity)>QUANTITY_MAX_SIZE_STRING)
		return false;

	for (int i=0; stringQuantity[i]!='\0'; i++)
		if (stringQuantity[i]<'0' || stringQuantity[i]>'9')
			return false;

	if (stringToInt(stringQuantity)<0 || stringToInt(stringQuantity)>QUANTITY_MAX_SIZE_INT)
		return false;

	return true;
}

bool checkConfirmation(char confirmation[]){
	if ( !strcmp(confirmation, "SIM"))
		return true;
	else if ( !strcmp(confirmation, "S"))
		return true;
	else if ( !strcmp(confirmation, "sim"))
		return true;
	else if ( !strcmp(confirmation, "s"))
		return true;
	else
		return false;

	return false;
}

bool checkDay(char day[]){
	if (strlen(day)>2 || strlen(day)==0){
		return false;
	}
	else if (strlen(day)==1){
		if (day[0]=='0')
			return false;
	}
	else if (strlen(day)==2){
		if (day[0]=='0' && day[1]=='0')
			return false;
		else if (day[0]=='3' && day[1]>'1')
			return false;
		else if (day[0]>'3')
			return false;
	}

	if (strlen(day)==1){      //meter 0 atras para comparar mais facilmente
		day[2]='\0';
		day[1]=day[0];
		day[0]='0';
	}

	return true;
}

bool checkMonth(char month[]){
	if (strlen(month)>2 || strlen(month)==0){
		return false;
	}
	else if (strlen(month)==1){
		if (month[0]=='0')
			return false;
	}
	else if (strlen(month)==2){
		if (month[0]=='0' && month[1]=='0')
			return false;
		else if (month[0]=='1' && month[1]>'2')
			return false;
		else if (month[0]>'1')
			return false;
	}

	if (strlen(month)==1){
		month[2]='\0';
		month[1]=month[0];
		month[0]='0';
	}

	return true;
}

bool checkYear(char year[]){
	if (strlen(year)!=4)
		return false;

	for (int i=0; year[i]!='\0'; i++)
		if (year[i]<'0' || year[i]>'9')
			return false;

	return true;
}

//original = advancedlinuxprogramming.com/listings/chapter-8/lock-file.c
/*Avé Google, StackOverflow, AdvancedLinuxProgramming e 1 dia inteiro por estas duas maravilhas!!!
Metade nosso (desenvolvedores), metade internet <3 */
struct fileStructure openFile(char fileToEdit[], char mode[]){

	struct fileStructure fileStruct = {0, (struct flock *)NULL, (FILE *)NULL};

	int fd;
	struct flock lock;


	//modo de leitura
	if ( !strcmp(mode, "READ")){

		/*abre um file descriptor para o ficheiro que queremos e mete um pointer para o ficheiro na estrutura de ficheiros
		O_RDONLY = read-only
		O_WRONLY = write-only
		O_RDWR = read/write */
		//http://codewiki.wikidot.com/c:system-calls:open
		fd = open(fileToEdit, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		
		/*passar um file descriptor para um file stream
		r 	Opens an existing text file for reading purpose.
		
		w 	Opens a text file for writing. If it does not exist, 
		then a new file is created. Here your program will start writing content from the beginning of the file.
		
		a 	Opens a text file for writing in appending mode. 
		If it does not exist, then a new file is created. 
		Here your program will start appending content in the existing file content.
		
		r+ 	Opens a text file for both reading and writing.
		
		w+ 	Opens a text file for both reading and writing. 
		It first truncates the file to zero length if it exists, otherwise creates a file if it does not exist.
		
		a+ 	Opens a text file for both reading and writing. I
		t creates the file if it does not exist. 
		The reading will start from the beginning but writing can only be appended.*/
		fileStruct.filePointer = fdopen(fd, "r");

		//iniciar a estrutura do flock
		memset(&lock, 0, sizeof(lock));

		/*F_RDLCK = lock de leitura
		F_WRLCK = lock de escrita
		F_UNLCK = tirar o lock (unlock)*/
		lock.l_type = F_RDLCK;
	}
	//modo de escrita
	else if ( !strcmp(mode, "WRITE")){
		//O_TRUNC = eliminar tudo o que está dentro do ficheiro
		fd = open(fileToEdit, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		fileStruct.filePointer = fdopen(fd, "r+");
		memset(&lock, 0, sizeof(lock));
		lock.l_type = F_WRLCK;
	}
	//modo de acrescentar ao final
	else if ( !strcmp(mode, "APPEND")){
		fd = open(fileToEdit, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		fileStruct.filePointer = fdopen(fd, "a");
		memset(&lock, 0, sizeof(lock));
		lock.l_type = F_WRLCK;
	}
	else {
		cleanScreen();
		fputs("Erro! Modo de abrir ficheiro não reconhecido.\n\n\n", stderr);
		fileStruct.filePointer=NULL;
		return fileStruct;
	}
	
	//meter um lock no ficheiro e ao mesmo tempo verificar se dá para meter e caso passe 3 segundos sem poder meter, então dar erro
	int cnt=0;
	while (fcntl(fd, F_SETLK, &lock)==-1){
		sleep(1);
		cnt++;
		if (cnt>=3){
			cleanScreen();
			fprintf(stderr, "Não foi possível meter um lock no ficheiro \"%s\"\n\n\n", fileToEdit);
			fileStruct.filePointer=NULL;
			return fileStruct;
		}
	}

	//meter as informações nesta estrutura para usar noutras funções
	fileStruct.fd = fd;
	fileStruct.lock = &lock;

	if (!fileStruct.filePointer) {
		closeFile(fileStruct);
		cleanScreen();
		fprintf(stderr, "Erro! Não foi possível abrir o ficheiro \"%s\".\n\n\n", fileToEdit);
		return fileStruct;      //retornar na mesma a estrutura para fazer return também na função de chamada
	}

	return fileStruct;
}

void closeFile(struct fileStructure fileStruct){

	//retirar o lock

	/*F_RDLCK = lock de leitura
	F_WRLCK = lock de escrita
	F_UNLCK = tirar o lock (unlock)*/
	fileStruct.lock->l_type = F_UNLCK;

	fcntl(fileStruct.fd, F_SETLKW, fileStruct.lock);

	//se o ponteiro for nulo, não fazer isto ou dá falha de segmentação
	if (fileStruct.filePointer)
		fclose(fileStruct.filePointer);

	close(fileStruct.fd);
}

void logUser(char username[]){
	struct fileStructure file_userdb = openFile("userdb", "WRITE");
	struct fileStructure file_temp = openFile("temp", "APPEND");

	struct userData tempUserData;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
		tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged)!=EOF) {

		if ( !strcmp(tempUserData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;TRUE\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}

	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
}

void logoutUser(char username[]){
	struct fileStructure file_userdb = openFile("userdb", "WRITE");
	struct fileStructure file_temp = openFile("temp", "APPEND");

	struct userData tempUserData;

	while (fscanf(file_userdb.filePointer, "%[^;];%[^;];%[^;];%[^;];%[^;];%f;%[^;];%s\n", tempUserData.name, tempUserData.username, 
		tempUserData.password, tempUserData.email, tempUserData.contact, &tempUserData.balance, tempUserData.perms, tempUserData.logged)!=EOF) {

		if ( !strcmp(tempUserData.username, username))
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;FALSE\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms);

		else
			fprintf(file_temp.filePointer, "%s;%s;%s;%s;%s;%f;%s;%s\n", tempUserData.name, tempUserData.username, tempUserData.password, 
				tempUserData.email, tempUserData.contact, tempUserData.balance, tempUserData.perms, tempUserData.logged);
	}

	rename("temp", "userdb");
	closeFile(file_userdb);
	closeFile(file_temp);
}

void generateCode(char code[]){
	srand( (unsigned int)time(NULL));      //usar o srand com uma seed para gerar números aleatórios

	for (int i=0; i<9; i++)      //gerar números de 0 a 9 para cada posição do código
		code[i]= (char)(rand()%10 + '0');
			
	code[9]='\0';
}

bool getString(char string[], int size){
	char *fgr;
	bool result = true;      //Assumir como tudo correto

	fgr = fgets(string, size, stdin);

	if (fgr != NULL){
		if (string[strlen(string)-1] != '\n')
			flushInput();
	}
	else
		result = false;      //fgets retornou um erro ou leu EOF sem ler mais nada

	removeNewLine(string);      //transformar '\n' para '\0'

	return result;      //valor de retorno caso queiramos fazer debugging
}

void flushInput(){
	int ch;      //a função getchar retorna um int
	ch = getchar();

	while ( ch!=EOF && ch!='\n' )
		ch = getchar();      //ler caracteres até ao fim do buffer

	clearerr(stdin);      //limpar o EOF e indicador de erros que está no final do buffer stdin
}

void removeNewLine(char string[]){
	for (int i=0; string[i]!='\0'; i++)
		if (string[i]=='\n')
			string[i]='\0';
}

void cleanScreen(){
	for (int i=0; i<100; i++)
		fputs("\n", stdout);
}

float stringToFloat(char string[]){
	int dotpos=0;
	int len = (int)strlen(string);
	float number=0.0;
	
	for (int i = 0; i < len; i++) {
		if (string[i] == '.') {
			dotpos = len - i - 1;
		}
		else {
			number = number * 10.0f + (float)(string[i]-'0');
		}
	}

	while ( dotpos--) {
		number /= 10.0f;
	}

	return number;
}

int stringToInt(char string[]){
	int quantity = atoi(string);

	return quantity;
}

void about(){
	puts("Cliente de SuperMercado.\n");
	puts("Feito por Frederico Emanuel e Fernando Silva no âmbito do projeto da cadeira");
	puts("de 1º ano, Laboratório de Computadores, da Licenciatura em Ciência de");
	puts("Computadores e Mestrado Integrado em Engenharia de Redes e Sistemas Informáticos");
	puts("na mui nobre Faculdade de Ciências da Universidade do Porto.\n\n");
	puts("Frederico Emanuel Almeida Lopes, up201604674");
	puts("Fernando Daniel Sousa Ventura da Silva, up201604670\n\n\n");
}
