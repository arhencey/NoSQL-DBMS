# NoSQL-DBMS

This project is a DBMS written in C to implement the operations required to process and execute NoSQL queries on a file of data.  The data in the file corresponds to a collection of documents in a database.  The program processes the query by parsing it to identify which operations it is requesting, then performs the operations on the specified documents and displays the results.  The code implements operations similar to a select operation, projection, aggregate operations and insert that would be used by a NoSQL DBS.

The collection is named “final” and you must use this name, since it will be referenced in all queries.  In the data file each line represents a different document.  Since this is a NoSQL database the names of each field are included in the data along with the value of the field.  The fields may be stored in a different order for each document.  Each field is a fieldName:value  - note the fieldName is followed by a colon then the value, and different fields are separated by a space. Assume all values for a field are integer.  The fields may not be in the same order for all documents in the input.  The documents always have a key field named DocID. If DocID is not included in the input data, its values are automatically generated by the system. The generated DocID is be current existing maximum DocID plus one.

Example of possible input data for collection final:  
DocID:555 Dept:5 Age:20  
Dept:10 Manager:555 DocID:777 Age:30  
DocID:666 Dept:7 Age:20  
Age:25 Manager:555 DocID:222  
DocID:333 Dept:7 Age:21

The data is read from a file called data.txt.

Queries are read from a file called queries.txt.

The NoSQL query for this project is similar to MongoDB but it is NOT EXACTLY the same.  It supports the following operations.

Operation 1-
query: returns values to fields specified for documents that satisfy the specified condition(s).

final.query([condition],[field])

condition -- fieldNameComparisonOperatorValue
             There will be zero or more select conditions. Zero conditions is denoted as empty [] and it means include all documents.  If there is more than one condition, conditions will be separated comma (meaning "and" operation).  There will not be any "or" operations.  The ComparisonOperator can be <, >, =, <=, >=, <>. 

field -- fieldName    
            Zero or more field names separated by commas. This is a projection operation.  Zero fields is denoted as the empty list [] and it means include all fields in each document that satisfies the condition.  Unlike MongoDB the DocID field will only be displayed if requested. If a document satisfying the conditions does not have one (or more) of the fields in the fieldName list, its value (or their values) will be printed as NA.


final.query([],[]) returns the entire collection, all fields (except for DocID) for all documents.

Output:  For each document satisfying the conditions, output each field name ending with a colon, followed by the value for the field.  Please separate different fields with a space.  Print the fields in the same order as the fieldName list. If fieldName is empty [] (all fields except for DocID are to be printed), you can print them in any order as long they are printed in the same order for all documents. 

Example queries and their results:


>final.query([],[DocID,Age])

Output:  
DocID:555 Age:20  
DocID:777 Age:30  
DocID:666 Age:20  
DocID:222 Age:25  
DocID:333 Age:21



>final.query([Manager=555],[DocID,Dept])

Output:  
DocID:777 Dept:10  
DocID:222 Dept:NA


>final.query([Age<30,Manager=555], [])

Output:  
Age:25 Manager:555

Since this is a NoSQL DB, there should not be an error generated if there is no match for the name of a collection or a field. In other words, if there is no such collection, output nothing.  If no documents satisfy the specified condition, output nothing. If there are conditions specified, a document must satisfy all conditions to be in the output. If "query" or any of the operations below is misspelled, print an error “query semantic error!”.

Example queries and their results:
>final.query([Age=6],[DocID])
Its output should be nothing.

Operation 2-
count:  an aggregate function which computes the count (number of occurrences) of the specified field for the versions indicated.

final.count([field],[unique])

field is one fieldName

unique is either 0 or 1. 1 means counting only unique field values. 0 means counting all field values.

Output: count_field:<number of items with the specified field>
If no such field, return nothing.  Obviously, do not include a document in the calculation if it does not contain the field specified for the aggregate.

Example:
>final.count([Manager],[0])

Output:  
count_Manager:2

 

Example:
>final.count([Manager],[1])

Output:  
count_Manager:1

Operation 3-
insert: a method which inserts new documents into the database. Field value pairs are separated by a space. If the values for the key field (DocID) does not exist in the collection, then the system will automatically generate a value that equals to the current max DocID value plus 1. Note that a new field name is allowed. If a document is inserted successfully, the system will print all field value pairs separated by space in the same format as in Operation 1 (query), except that the DocID field should always be printed at the beginning. In contrast, the system should print “Duplicate DocID error!” when inserting a document with a DocID value that already exists.

final.insert(field1:value field2:value field3:value …)

Example:

>final.insert(Age:23 Manager:434 Salary:10000)

Output:  
DocID:778 Age:23 Manager:434 Salary:10000

The document will be inserted into the database and will be available for subsequent queries and functions.

 

Another Example:

>final.insert(DocID:222 Age:30)

Output:  
Duplicate DocID error!


