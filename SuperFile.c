#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "General.h"
#include "FileHelper.h"
#include "SuperFile.h"
#include "Product.h"
#include "myMacros.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");

	CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n")


	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		CLOSE_RETURN_0(fp)
	

	if (!writeIntToFile(pMarket->productCount, fp, "Error write product count\n"))
		CLOSE_RETURN_0(fp)
	

	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (!saveProductToFile(pMarket->productArr[i], fp))
			CLOSE_RETURN_0(fp)
	}

	fclose(fp);

	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}

int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");

	CHECK_MSG_RETURN_0(fp, "Error open company file\n")
	

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
		CLOSE_RETURN_0(fp)
	

	int count;

	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
	

	pMarket->productArr = (Product**)malloc(count * sizeof(Product*));
	if (!pMarket->productArr)
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
	

	pMarket->productCount = count;

	for (int i = 0; i < count; i++)
	{
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		if (!pMarket->productArr[i])
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)
		

		if (!loadProductFromFile(pMarket->productArr[i], fp))
		{
			free(pMarket->productArr[i]);
			free(pMarket->name);
			fclose(fp);
			return 0;
		}
	}


	fclose(fp);

	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}

int	saveCustomersToTextFile(const Customer* customerArr, int customerCount, const char* customersFileName)
{
	FILE* fp;

	fp = fopen(customersFileName, "w");
	if (!fp) {
		printf("Error opening customers file to write\n");
		return 0;
	}

	fprintf(fp, "%d\n", customerCount);
	for (int i = 0; i < customerCount; i++)
		customerArr[i].vTable.saveToFile(&customerArr[i], fp);

	fclose(fp);
	return 1;
}

Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount)
{
	FILE* fp;

	fp = fopen(customersFileName, "r");
	if (!fp) {
		printf("Error open customers file to write\n");
		return NULL;
	}

	Customer* customerArr = NULL;
	int customerCount;

	fscanf(fp, "%d\n", &customerCount);

	if (customerCount > 0)
	{
		customerArr = (Customer*)calloc(customerCount, sizeof(Customer)); //cart will be NULL!!!
		if (!customerArr)
		{
			fclose(fp);
			return NULL;
		}

		for (int i = 0; i < customerCount; i++)
		{
			if (!loadCustomerFromFile(&customerArr[i], fp))
			{
				freeCustomerCloseFile(customerArr, i, fp);
				return NULL;
			}
		}
	}

	fclose(fp);
	*pCount = customerCount;
	return customerArr;
}

void freeCustomerCloseFile(Customer* customerArr, int count, FILE* fp)
{
	for (int i = 0; i < count; i++)
	{
		free(customerArr[i].name);
		customerArr[i].name = NULL;
		if (customerArr[i].pDerivedObj)
		{
			free(customerArr[i].pDerivedObj);
			customerArr[i].pDerivedObj = NULL;
		}
	}
	free(customerArr);
	fclose(fp);
}


int	loadSuperMarketFromCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");

	CHECK_MSG_RETURN_0(fp, "Error open company file\n")

	BYTE data1[2];
	if (fread(&data1, sizeof(BYTE), 2, fp) != 2)
		return 0;

	pMarket->productCount = data1[0] << 2 | data1[1] >> 6;
	int superMarketNameLen = data1[1] & 0x3F;

	pMarket->name = readNameFromCompressedFile(fp, superMarketNameLen , "Error reading supermarket name\n");
	if (!pMarket->name)
		CLOSE_RETURN_0(fp)

	pMarket->productArr = (Product**)malloc(pMarket->productCount * sizeof(Product*));
	if (!pMarket->productArr)
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

	for (int i = 0; i < pMarket->productCount; i++)
	{
		pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
		if (!pMarket->productArr[i])
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp)

			if (!loadProductFromCompressedFile(pMarket->productArr[i], fp))
			{
				free(pMarket->productArr[i]);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
	}
	fclose(fp);

	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;
}


int	saveSuperMarketToCompressedFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");

	CHECK_MSG_RETURN_0(fp, "Error open supermarket file to write\n")

	BYTE data1[2];
	int marketNameLen = (int)strlen(pMarket->name);
	
	data1[0] = pMarket->productCount >> 2;
	data1[1] = ((pMarket->productCount & 0x3) << 6) | marketNameLen;

	if (fwrite(&data1, sizeof(BYTE), 2, fp) != 2)
		CLOSE_RETURN_0(fp)

	if (fwrite(pMarket->name, sizeof(char), marketNameLen, fp) != marketNameLen)
		CLOSE_RETURN_0(fp)

	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (!saveProductToCompressedFile(pMarket->productArr[i], fp))
			CLOSE_RETURN_0(fp)
	}


	fclose(fp);

	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}
