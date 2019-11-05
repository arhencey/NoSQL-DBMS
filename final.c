/*
 * Final project for CS301
 * Written by Alan Hencey
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Row
{
	char field[256][128];
	int value[128];
	int numFields;
} row;

int getNumRows();
void getMaxDocID();
void processQueries();
int checkQueryStartsWithFinal(FILE *fp);
void doQuery(FILE *fp);
void doCount(FILE *fp);
void doInsert(FILE *fp);
int doesDocIDalreadyExist(int docID);
int getOperation(FILE *fp);

int maxDocID = 0;

int main(void)
{
	processQueries();
}

int getNumRows()
{
	FILE *fp = fopen("data.txt","r");
	if (!fp)
	{
		printf("couldnt open file\n");
		return -1;
	}

	char c;
	int numRows = 0;
	for (c = getc(fp); c != EOF; c = getc(fp))
	{
		if (c == '\n') numRows++;
	}

	fclose(fp);
	return numRows;
}

void getMaxDocID()
{
	FILE *fp = fopen("data.txt","r");
	char next = '\0';
	char *p = "";
	char rawString[128] = "";
	memset(&rawString[0], 0, sizeof(rawString));
	while (next != EOF && fscanf(fp,"%s",rawString) != EOF)
	{
		next = getc(fp);
		p = strtok(rawString, ":");
		if (strcmp(p,"DocID") == 0)
		{
			p = strtok(NULL, ":");
			if (atoi(p) > maxDocID) maxDocID = atoi(p);
		}
	}
	fclose(fp);
}

void processQueries()
{
	getMaxDocID();
	FILE *fp = fopen("queries.txt","r");
	int op = 0;

	while (checkQueryStartsWithFinal(fp) != -1)
	{
		op = getOperation(fp);
		if (op == -1) printf("Query semantic error!\n\n");
		else if (op == 1)
		{
			doQuery(fp);
			printf("\n");
		}
		else if (op == 2)
		{
			doCount(fp);
			printf("\n");
		}
		else
		{
			doInsert(fp);
			printf("\n");
		}
	}

	fclose(fp);
}

//Returns 1 if all good, 0 if not "final." and -1 if EOF
int checkQueryStartsWithFinal(FILE *fp)
{
	char everythingBeforePeriod[128];
	int i = 0;
	everythingBeforePeriod[i] = getc(fp);
	if(everythingBeforePeriod[i] == '\n')
	{
		everythingBeforePeriod[i] = getc(fp);
	}
	if (everythingBeforePeriod[i] == EOF) return -1;
	while (everythingBeforePeriod[i] != '.')
	{
		i++;
		everythingBeforePeriod[i] = getc(fp);
	}
	if (strcmp(everythingBeforePeriod,"final.") == 0) return 1;
	else return 0;
}

// Returns 1 for query, 2 for count, 3 for insert, and -1 for spelling error
int getOperation(FILE *fp)
{
	char op[16] = "";
	for (int i = 0; i < 6; ++i)
	{
		op[i] = getc(fp);
	}
	if (strcmp(op,"query(") == 0) return 1;
	else if (strcmp(op,"count(") == 0) return 2;
	else
	{
		op[6] = getc(fp);
		if (strcmp(op,"insert(") == 0) return 3;
		else return -1;
	}
}

void doQuery(FILE *fp)
{
	//Get query conditions
	char next = getc(fp); // '['
	char queryField[256][128];
	memset(&queryField[0], 0, sizeof(queryField));
	int queryOperation[256];
	memset(&queryOperation[0], 0, sizeof(queryOperation));
	int queryValue[256];
	memset(&queryValue[0], 0, sizeof(queryValue));
	char valueAsChar[32];
	memset(&valueAsChar[0], 0, sizeof(valueAsChar));
	int numConditions = 0;
	int i = 0;
	int j = 0;
	while (next != ']')
	{
		i = 0;
		while (next != '<' && next != '>' && next != '=')
		{
			next = getc(fp);
			if (next != '<' && next != '>' && next != '=')
			{
				if (next == ']') break;
				else
				{
					queryField[numConditions][i] = next;
					i++;
				}
			}
			//i++;
		}
		if (next == ']') break;
		else if (next == '=')
		{
			queryOperation[numConditions] = 3;
			next = getc(fp);
			j = 0;
			while (next != ',' && next != ']')
			{
				valueAsChar[j] = next;
				next = getc(fp);
				j++;
			}
			valueAsChar[j] = '\0';
			queryValue[numConditions] = atoi(valueAsChar);
		}
		else if (next == '<')
		{
			next = getc(fp);
			if (next == '=') queryOperation[numConditions] = 4;
			else if (next == '>') queryOperation[numConditions] = 6;
			else queryOperation[numConditions] = 1;
				j = 0;
				while (next != ',' && next != ']')
				{
					if (next != '=' && next != '>')
					{
						valueAsChar[j] = next;
						j++;
					}
					next = getc(fp);
				}
				valueAsChar[j] = '\0';
				queryValue[numConditions] = atoi(valueAsChar);
			//}
		}
		else //next == '>'
		{
			next = getc(fp);
			if (next == '=') queryOperation[numConditions] = 5;
			else
			{
				queryOperation[numConditions] = 2;
			}
				j = 0;
				while (next != ',' && next != ']')
				{
					if (next != '=')
					{
						valueAsChar[j] = next;
						j++;
					}
					next = getc(fp);
				}
				valueAsChar[j] = '\0';
				queryValue[numConditions] = atoi(valueAsChar);
			//}
		}
		numConditions++;
	}

	/*
	//Print query conditions
	printf("\nQuery conditions: %d\n",numConditions);
	for (int i = 0; i < numConditions; ++i)
	{
		printf("%s, %d, %d\n",queryField[i],queryOperation[i],queryValue[i]);
	}
	*/

	//Get fields to display
	char showFields[256][128];
	memset(&showFields[0], 0, sizeof(showFields));
	int numShowFields = 0;
	next = getc(fp);
	next = getc(fp); // '['
	while (next != ']')
	{
		i = 0;
		next = getc(fp);
		if (next == ']') break;
		while (next != ',' && next != ']')
		{
			showFields[numShowFields][i] = next;
			next = getc(fp);
			i ++;
		}
		numShowFields++;
	}
	getc(fp); //get ')'

	char *p;
	next = '\0';
	char rawString[128];
	memset(&rawString[0], 0, sizeof(rawString));

	/*
	//Print fields to display
	printf("Fields to display: %d\n",numShowFields);
	for (int i = 0; i < numShowFields; ++i)
	{
		printf("%s\n",showFields[i]);
	}
	*/

	//Get rows to display
	FILE *datafp = fopen("data.txt","r");
	memset(&rawString[0], 0, sizeof(rawString));
	p = "a";
	int curRow = 1;
	int rowsToDisplay[256];
	memset(&rowsToDisplay[0], 0, sizeof(rowsToDisplay));
	int numRowsToDisplay = 0;
	next = ' ';
	i = 0;
	int rowAlreadyAdded = 0;
	int numConditionsMet = 0;
	if (numConditions == 0)
	{
		while (next != EOF)
		{
			next = getc(datafp);
			if (next == '\n')
			{
				i++;
				rowsToDisplay[numRowsToDisplay++] = i;
			}
		}
	}
	else
	{
		while (next != -1 && fscanf(datafp,"%s",rawString) != EOF)
		{
			p = strtok(rawString, ":");
			for (int j = 0; j < numConditions; ++j)
			{
				if (strcmp(p,queryField[j]) == 0)
				{
					p = strtok(NULL, ":");
					if (queryOperation[j] == 1)
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) < queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					else if (queryOperation[j] == 2)
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) > queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					else if (queryOperation[j] == 3)
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) == queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					else if (queryOperation[j] == 4)
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) <= queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					else if (queryOperation[j] == 5)
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) >= queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					else //queryOperation[j] == 6
					{
						for (int k = 0; k < numRowsToDisplay; ++k)
						{
							if (rowsToDisplay[k] == curRow)
							{
								rowAlreadyAdded = 1;
							}
						}
						if (atoi(p) != queryValue[j])
						{
							if (rowAlreadyAdded == 0)
							{
								numConditionsMet++;
							}
						}
						else
						{
							if (rowAlreadyAdded == 1)
							{
								for (int k = 0; k < numRowsToDisplay; ++k)
								{
									if (rowsToDisplay[k] == curRow)
									{
										for (int l = k; l < numRowsToDisplay; ++l)
										{
											rowsToDisplay[l] = rowsToDisplay[l+1];
											numRowsToDisplay--;
										}
									}
								}
								rowAlreadyAdded = 0;
							}
						}
					}
					break;
				}
			}
			memset(&rawString[0], 0, sizeof(rawString));
			next = getc(datafp);
			if (next == '\n')
			{
				if (numConditionsMet == numConditions)
				{
					rowsToDisplay[numRowsToDisplay++] = curRow;
				}
				numConditionsMet = 0;
				curRow++;
			}
		}
	}

	/*
	//Print rows to display
	printf("numRowsToDisplay: %d\n",numRowsToDisplay);
	printf("Row numbers that meet conditions: %d\n",numRowsToDisplay);
	for (int i = 0; i < numRowsToDisplay; ++i)
	{
		printf("%d\n",rowsToDisplay[i]);
	}
	printf("\n");
	*/

	//Perform query
	if (numConditions == 0)
	{
		fseek(datafp,0,0);
		memset(&rawString[0], 0, sizeof(rawString));
		char rs2[128] = "";
		memset(&rs2[0], 0, sizeof(rs2));
		next = '\0';
		if (numShowFields == 0)
		{
			while (next != EOF && fscanf(datafp,"%s",rawString) != EOF)
			{
				next = getc(datafp);
				strcpy(rs2,rawString);
				p = strtok(rs2, ":");
				if (strcmp(p,"DocID") != 0)
				{
					printf("%s",rawString);
					if (next != EOF) printf("%c",next);
				}
				else if (next == '\n') printf("%c",next);
			}
		}
		else
		{
			int pos = 0;
			int foundFields[256] = {0};
			fscanf(datafp,"%s",rawString);
			next = getc(datafp);
			p = strtok(rawString, ":");
			while (next != EOF)
			{
				for (int k = 0; k < numShowFields; ++k)
				{
					while (1)
					{
						if (strcmp(p,showFields[k]) == 0)
						{
							foundFields[k] = 1;
							printf("%s:",p);
							p = strtok(NULL, ":");
							printf("%s",p);
							if (k == numShowFields - 1) printf("\n");
							break;
						}
						if (next == '\n')
						{
							printf("%s:NA",showFields[k]);
							if (k == numShowFields - 1) printf("\n");
							break;
						}
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
					if (k != numShowFields - 1) //goto line start
					{
						printf(" ");
						fseek(datafp,pos,0);
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
					else
					{
						while (next != '\n') next = getc(datafp);
						pos = ftell(datafp);
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
				}
			}
		}
	}

	else
	{
		fseek(datafp,0,0);
		memset(&rawString[0], 0, sizeof(rawString));
		int curLine = 1;
		int foundFields[256] = {0};
		int pos = 0;

		pos = ftell(datafp);
		fscanf(datafp,"%s",rawString);
		next = getc(datafp);
		p = strtok(rawString, ":");
		for (int i = 0; i < numRowsToDisplay; ++i)
		{
			if (curLine != rowsToDisplay[i])
			{
				while (curLine != rowsToDisplay[i])
				{
					if (getc(datafp) == '\n') curLine++;
				}
				pos = ftell(datafp);
				memset(&rawString[0], 0, sizeof(rawString));
				/*fscanf(datafp,"%s",rawString);
				next = getc(datafp);
				p = strtok(rawString, ":");*/
			}
			//printf("curLine: %d    ",curLine);

			if (numShowFields == 0)
			{
				next = '\0';
				char rs3[128] = "";
				memset(&rs3[0], 0, sizeof(rs3));
				memset(&rawString[0], 0, sizeof(rawString));
				while (next != '\n' && fscanf(datafp,"%s",rawString) != EOF
															 && next != EOF)
				{
					next = getc(datafp);
					strcpy(rs3,rawString);
					if (strcmp(rs3,"DocID") != 0)
					{
						printf("%s",rawString);
						if (next != EOF) printf("%c",next);
					}
					else if (next == '\n') printf("%c",next);
				}
			}
			else
			{
				fscanf(datafp,"%s",rawString);
				next = getc(datafp);
				p = strtok(rawString, ":");
				for (int k = 0; k < numShowFields; ++k)
				{
					while (1)
					{
						if (strcmp(p,showFields[k]) == 0)
						{
							foundFields[k] = 1;
							printf("%s:",p);
							p = strtok(NULL, ":");
							printf("%s",p);
							if (k == numShowFields - 1) printf("\n");
							break;
						}
						else if (next == '\n')
						{
							printf("%s:NA",showFields[k]);
							if (k == numShowFields - 1) printf("\n");
							break;
						}
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
					if (k != numShowFields - 1) //goto line start
					{
						printf(" ");
						fseek(datafp,pos,0);
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
					else
					{
						while (next != '\n') next = getc(datafp);
						pos = ftell(datafp);
						memset(&rawString[0], 0, sizeof(rawString));
						fscanf(datafp,"%s",rawString);
						next = getc(datafp);
						p = strtok(rawString, ":");
					}
				}
			}
			curLine++;
		}
	}

	fclose(datafp);
}

void doCount(FILE *fp)
{
	//Get field and unique values
	int i = 0;
	char fieldName[128] = "";
	char c = getc(fp);
	c = getc(fp);
	while (c != ']')
	{
		fieldName[i] = c;
		c = getc(fp);
		i++;
	}

	for (i = 0; i < 3; ++i) c = getc(fp);
	getc(fp);
	getc(fp);
	getc(fp);
	int unique = c - '0';

	//Count the values in table
	int count = 0;
	FILE *datafp = fopen("data.txt","r");
	char rawString[128];
	char *p;
	char next = '\0';
	i = 0;
	int uniqueVals[128];
	int sizeUniqueVals = 0;
	int flag = 0;

	if (unique == 0)
	{
		while (next != -1 && fscanf(datafp,"%s",rawString) != EOF)
		{
			//fscanf(datafp,"%s",rawString);
			p = strtok(rawString, ":");
			if (strcmp(fieldName,p) == 0) count++;
			p = strtok(NULL, ":");
			next = fgetc(datafp);
			i++;
		}
		fclose(datafp);
		printf("count_%s:%d\n",fieldName,count);
	}
	else if (unique == 1)
	{
		while (next != -1 && fscanf(datafp,"%s",rawString) != EOF)
		{
			p = strtok(rawString, ":");
			if (strcmp(fieldName,p) == 0)
			{
				p = strtok(NULL, ":");
				for (int x = 0; x < sizeUniqueVals; ++x)
				{
					if (atoi(p) == uniqueVals[x]) flag = 1;
				}
				if (flag != 1)
				{
					count++;
					uniqueVals[sizeUniqueVals++] = atoi(p);
				}
				flag = 0;
			}
			else
			{
				p = strtok(NULL, ":");
			}
			next = fgetc(datafp);
			i++;
		}
		fclose(datafp);
		printf("count_%s:%d\n",fieldName,count);
	}
}

void doInsert(FILE *fp)
{
	int numRows = getNumRows();
	char next = '\0';
	char *p;
	char rawString[128];
	char givenFields[256][128];
	int givenValues[256];
	int numFieldsGiven = 0;
	int docIDgiven = 0;
	int givenDocID;
	while (next != '\n' && next != EOF)
	{
		fscanf(fp,"%s",rawString);
		p = strtok(rawString, ":");
		strcpy(givenFields[numFieldsGiven],p);
		if (strcmp("DocID",p) == 0)
		{
			p = strtok(NULL, ":");
			givenValues[numFieldsGiven] = atoi(p);
			givenDocID = atoi(p);
			docIDgiven = 1;
		}
		else
		{
			p = strtok(NULL, ":");
			givenValues[numFieldsGiven] = atoi(p);
		}
		next = fgetc(fp);
		numFieldsGiven++;
	}

	if (docIDgiven)
	{
		if (doesDocIDalreadyExist(givenDocID))
		{
			printf("Duplicate DocID error!\n");
			return;
		}
	}
	else
	{
		int newDocID = maxDocID + 1;
		maxDocID++;
		strcpy(givenFields[numFieldsGiven],"DocID");
		givenValues[numFieldsGiven] = newDocID;
		numFieldsGiven++;
	}

	//Write new fields and values to data.txt
	FILE *datafp = fopen("data.txt","a");
	for (int i = 0; i < numFieldsGiven; ++i)
	{
		fprintf(datafp,"%s:%d",givenFields[i],givenValues[i]);
		if (i < numFieldsGiven - 1) fprintf(datafp," ");
	}
	fprintf(datafp,"\n");
	fclose(datafp);
	numRows++;

	//Print out newly inserted tuple
	int docIDindex = 0;
	for (int i = 0; i < numFieldsGiven; ++i)
	{
		if (strcmp(givenFields[i],"DocID") == 0)
		{
			docIDindex = i;
			break;
		}
	}
	printf("DocID:%d",givenValues[docIDindex]);
	for (int i = 0; i < numFieldsGiven; ++i)
	{
		printf(" ");
		if (i != docIDindex)
		{
			printf("%s:%d",givenFields[i],givenValues[i]);
		}
	}
	printf("\n");
}

// Returns 1 if DocID already exists, 0 if it does not
int doesDocIDalreadyExist(int docID)
{
	FILE *datafp = fopen("data.txt","r");
	char next = '\0';
	char *p;
	char rawString[128];
	while (next != EOF)
	{
		fscanf(datafp,"%s",rawString);
		p = strtok(rawString, ":");
		if (strcmp("DocID",p) == 0)
		{
			p = strtok(NULL, ":");
			if (docID == atoi(p))
			{
				fclose(datafp);
				return 1;
			}
			else
			{
				p = strtok(NULL, ":");
				next = fgetc(datafp);
				if (next != EOF)
				{
					next = getc(datafp);
					if (next == EOF) break;
					else next = ungetc(next,datafp);
				}
			}
		}
		else
		{
			p = strtok(NULL, ":");
			next = fgetc(datafp);
			if (next != EOF)
			{
				next = getc(datafp);
				if (next == EOF) break;
				else next = ungetc(next,datafp);
			}
		}
	}
	fclose(datafp);
	return 0;
}
