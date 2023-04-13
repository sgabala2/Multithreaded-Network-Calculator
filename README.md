Joy Neuberger and Shaina Gabala
jneuber6@jhu.edu, sgabala2@jhu.edu

# Multithreaded Network Calculator
This is a network-based app that allows the user to enter expressions and get the printed result.

Program:
* has a protocol for communication between clients and server.
* allows communication over a network.
* uses threads to handle concurrent client connections
* uses synchronization to allow multiple threads to access shared data

## Overview
To make the calculator instance's shared data safe to access from multiple threads, we used a mutex field to our calc struct. As the calculator is constructed, we initialize the field, and we destroy it as the calculator is destructed.

To ensure synchronization and prevent errors, we prevent concurrent access to the calculator instance whenever there is a chance that the state of the instance might be changed. In this context, that means we prevent concurrent access whenever a variable might be created or have its value changed. Since this only occurs in the evalExpr function, we lock the mutex field before the function is called, and unlock it upon return. This way, we are sure that changes made within an expression calculation lock our dictionary and make it safe to find, access, and add variables to it.

## Test it out
Here are some automated tests you can try.

### Download the test files
Download the following files into the directory containing your calcServer executable from a terminal by running the following commands:

```
curl -O https://jhucsf.github.io/spring2020/assign/assign06/test_server_concurrent1.sh
curl -O https://jhucsf.github.io/spring2020/assign/assign06/test_server_concurrent2.sh
curl -O https://jhucsf.github.io/spring2020/assign/assign06/test_server_concurrent_stress.sh
curl -O https://jhucsf.github.io/spring2020/assign/assign06/test_input.txt
curl -O https://jhucsf.github.io/spring2020/assign/assign06/conc_test_input1.txt
curl -O https://jhucsf.github.io/spring2020/assign/assign06/conc_test_input2.txt

```

### Make the scripts executable:
```
chmod a+x test_server_concurrent1.sh
chmod a+x test_server_concurrent2.sh
chmod a+x test_server_concurrent_stress.sh
```

### First test: 
Run the following commands:
```
./test_server_concurrent1.sh 30000 test_input.txt actual1.txt
cat actual1.txt
```
The output of the cat command should be:
```
2
3
5
This test tests that a long-running client does not prevent the server from handling an additional client connection.
```
### Second test: 
Run the following commands:
```
./test_server_concurrent2.sh 30000 conc_test_input1.txt actual1.txt conc_test_input2.txt actual2.txt
cat actual1.txt
cat actual2.txt
```
The output of the first cat command should be:
```
1
42
```
The output of the second cat command should be:
```
40
54
```
This test tests that two client sessions can interact with each other through commands accessing a shared variable.

### Third test: 
Run the following commands:
```
./test_server_concurrent_stress.sh 30000
cat final_count.txt
```
The file final_count.txt does not need to contain any specific value, but it will probably be somewhere between `200000` and `400000`.

If you fully synchronized calc_eval, such that each expression is fully evaluated with the mutex held, the final count should be `400000`.

The most important outcome of this test is that your server should not crash.