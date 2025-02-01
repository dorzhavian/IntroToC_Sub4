#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "FileHelper.h"

#define MIN_DIG 3
#define MAX_DIG 5

static const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };
static const char* typePrefix[eNofProductType] = { "FV", "FR", "FZ", "SH" };


void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	generateBarcode(pProduct);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	initDate(&pProduct->expiryDate);
	pProduct->price = getPositiveFloat("Enter product price\t");
	pProduct->count = getPositiveInt("Enter product number of items\t");
}

void initProductName(Product* pProduct)
{
	do {
		printf("enter product name up to %d chars\n", NAME_LENGTH);
		myGets(pProduct->name, sizeof(pProduct->name), stdin);
	} while (checkEmptyString(pProduct->name));
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (fwrite(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error saving product to file\n");
		return 0;
	}
	return 1;
}

int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (fread(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error reading product from file\n");
		return 0;
	}
	return 1;
}

int	compareProductsByName(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return strcmp(pProd1->name, pProd2->name);
}

int	compareProductsByCount(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return pProd1->count - pProd2->count;
}

int	compareProductsByPrice(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	if (pProd1->price > pProd2->price)
		return 1;
	if (pProd1->price < pProd2->price)
		return -1;
	return 0;
}

void printProduct(const Product* pProduct)
{
	char* dateStr = getDateStr(&pProduct->expiryDate);
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %13d %7s %15s\n", typeStr[pProduct->type], pProduct->price, pProduct->count, " ", dateStr);
	free(dateStr);
}

void printProductPtr(void* v1)
{
	Product* pProduct = *(Product**)v1;
	printProduct(pProduct);
}

void generateBarcode(Product* pProd)
{
	char temp[BARCODE_LENGTH + 1];
	int barcodeNum;

	strcpy(temp, getProductTypePrefix(pProd->type));
	do {
		barcodeNum = MIN_BARCODE + rand() % (RAND_MAX - MIN_BARCODE + 1); //Minimum 5 digits
	} while (barcodeNum > MAX_BARCODE);
	
	sprintf(temp + strlen(temp), "%d", barcodeNum);

	strcpy(pProd->barcode, temp);
}

void getBarcodeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg, "Code should be of %d length exactly\n"
				 "Must have %d type prefix letters followed by a %d digits number\n"
				 "For example: FR20301",
				 BARCODE_LENGTH, PREFIX_LENGTH, BARCODE_DIGITS_LENGTH);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts("Invalid barcode length");
			ok = 0;
		}
		else
		{
			//check first PREFIX_LENGTH letters are upper case and valid type prefix
			char* typeSubStr = (char*)malloc(PREFIX_LENGTH + 1);
			if (!typeSubStr)
				return;
			strncpy(typeSubStr, temp, PREFIX_LENGTH);
			typeSubStr[PREFIX_LENGTH] = '\0';
			int prefixOk = 0;
			int i;

			for (i = 0; i < eNofProductType; i++)
			{
				if (strcmp(typeSubStr, typePrefix[i]) == 0)
				{
					prefixOk = 1;
					break; //found valid type prefix
				}
			}

			free(typeSubStr); //free the allocated memory

			if (!prefixOk)
			{
				puts("Invalid type prefix");
				ok = 0;
			}
			else
			{
				for (i = PREFIX_LENGTH; i < BARCODE_LENGTH; i++)
				{
					if (!isdigit(temp[i]))
					{
						puts("Only digits after type prefix\n");
						puts(msg);
						ok = 0;
						break;
					}
					digCount++;
				}

				if (digCount != BARCODE_DIGITS_LENGTH)
				{
					puts("Incorrect number of digits");
					ok = 0;
				}
			}
		}
	} while (!ok);

	strcpy(code, temp);
}

eProductType getProductType()
{
	int option;

	printf("\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);

	getchar();

	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

const char* getProductTypePrefix(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typePrefix[type];
}

int isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

void updateProductCount(Product* pProduct)
{
	int count;

	do {
		printf("How many items to add to stock? ");
		scanf("%d", &count);
	} while (count < 1);

	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}


int		loadProductFromCompressedFile(Product* pProduct, FILE* fp)
{
	BYTE data2[4];
	if (fread(&data2, sizeof(BYTE), 4, fp) != 4)
		return 0;
	char num1, num2, num3, num4, num5;
	int type, productNameLen;
	num1 = (data2[0] >> 4) + '0';
	num2 = (data2[0] & 0xF) + '0';
	num3 = (data2[1] >> 4) + '0';
	num4 = (data2[1] & 0xF) + '0';
	num5 = (data2[2] >> 4) + '0';
	type = data2[2] >> 2 & 0x3;
	productNameLen = 1 + ((data2[2] << 2 | data2[3] >> 6) & 0xF);

	strcpy(pProduct->barcode, getProductTypePrefix(type));
	char temp[6] = { num1, num2, num3, num4, num5, '\0'};
	strcat(pProduct->barcode, temp);

	pProduct->type = (eProductType)type;
	myGets(pProduct->name, productNameLen, fp);

	BYTE data3[3];
	if (fread(&data3, sizeof(BYTE), 3, fp) != 3)
	{
		//nothing to free
		return 0;
	}

	pProduct->count = data3[0];
	int agorot = data3[1] >> 1;
	int noAgorot = ((data3[1] & 0x1) << 8) | data3[2];
	pProduct->price = noAgorot + (agorot / 100.0f);

	BYTE data4[2];
	if (fread(&data4, sizeof(BYTE), 2, fp) != 2)
	{
		//nothing to free
		return 0;
	}

	pProduct->expiryDate.day = data4[0] >> 3;
	pProduct->expiryDate.month = ((data4[0] & 0x7) << 1) | (data4[1] >> 7);
	pProduct->expiryDate.year = 2024 + ((data4[1] >> 4) & 0x7);

	return 1;
}

int		saveProductToCompressedFile(Product* pProduct, FILE* fp)
{
	BYTE data2[4];

	int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, type, numBarcode, nameLen;
	type = pProduct->type;
	nameLen = (int)(strlen(pProduct->name));

	char barcodeNumbersStr[6] = {pProduct->barcode[2], pProduct->barcode[3], pProduct->barcode[4], pProduct->barcode[5], pProduct->barcode[6], '\0'};

	if (sscanf(barcodeNumbersStr, "%d", &numBarcode) != 1)
		return 0;

	num1 = numBarcode / 10000;
	num2 = (numBarcode / 1000) % 10;
	num3 = (numBarcode / 100) % 10;
	num4 = (numBarcode / 10) % 10;
	num5 = numBarcode % 10;

	data2[0] = (num1 << 4) | num2;
	data2[1] = (num3 << 4) | num4;
	data2[2] = (num5 << 4) | (type << 2) | (nameLen >> 2);
	data2[3] = nameLen << 6;

	if (fwrite(data2, sizeof(BYTE), 4, fp) != 4)
		return 0;

	int productNameLen = (int)strlen(pProduct->name);

	if (fwrite(pProduct->name, sizeof(char), productNameLen, fp) != productNameLen)
		return 0;

	BYTE data3[3];

	int agorot, noAgorot;
	noAgorot = (int)pProduct->price;
	agorot = (int)((pProduct->price - noAgorot) * 100);
	
	data3[0] = pProduct->count;
	data3[1] = (agorot << 1) | (noAgorot >> 8);
	data3[2] = noAgorot & 0xFF;

	if (fwrite(data3, sizeof(BYTE), 3, fp) != 3)
		return 0;

	BYTE data4[2];

	data4[0] = (pProduct->expiryDate.day << 3) | (pProduct->expiryDate.month >> 1);
	data4[1] = ((pProduct->expiryDate.month & 0x1) << 7) | ((2030 - pProduct->expiryDate.year) << 4);

	if (fwrite(data4, sizeof(BYTE), 2, fp) != 2)
		return 0;

	return 1;
}