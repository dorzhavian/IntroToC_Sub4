#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"

int main(int argc, char* argv[])
{
	SuperMarket	market;

	srand((unsigned int)time(NULL));

	if (argc != 3)
	{
		printf("Should enter 2 arguments\n");
		system("pause");
		return;
	}
	int opt;
	if (sscanf(argv[1], "%d", &opt) != 1)
	{
		printf("Should enter 0 for binary file or 1 for compressed binary file\n");
		system("pause");
		return;
	}
	if (opt != 1 && opt != 0)
	{
		printf("Should enter 0 for binary file or 1 for compressed binary file\n");
		system("pause");
		return;
	}


	if (!initSuperMarket(&market, argv[2], opt, CUSTOMER_FILE_NAME))
	{
		printf("error init Super Market");
		return 0;
	}

	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;

		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerManageShoppingCart:
			if(!manageShoppingCart(&market))
				printf("Error in shopping cart managment\n");
			break;

		case eSortProducts:
			sortProducts(&market);
			break;

		case eSearchProduct:
			findProduct(&market);
			break;

		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);

	if (strcmp(argv[2], SUPER_FILE_NAME) == 0)
	{
		if (!saveSuperMarketToFile(&market, argv[2], CUSTOMER_FILE_NAME))
			printf("Error saving supermarket to file\n");
	}
	else
	{
		if (!saveSuperMarketToCompressedFile(&market, argv[2], CUSTOMER_FILE_NAME))
			printf("Error saving supermarket to file\n");
	}

	freeMarket(&market);

	return 1;
}

int menu()
{
	int option;
	printf("\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

