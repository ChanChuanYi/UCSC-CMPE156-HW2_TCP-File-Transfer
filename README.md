# UCSC-CMPE156-HW2_TCP-File-Transfer
Project Description:

  The repository is the assignment 2 of course Network Programming

  Programs were developed in C language in Unix system.

  In this assignment, server and client sides of a simple remote shell application were developed

Functions:

  The problem in this assignment is to develop the client and server sides of an application that 
  enables a client to download “chunks” of a file from multiple servers (for example, ftp mirrors)
  distributed over the Internet, and assemble the chunks to form the complete file.

Error Handling:

  Pthread library was applied to implement client concurrency. At first, client parse the <server-info.txt> 
  file to acquire server IP address and port numbers. Client needs to handle the situation if:
  
    1.some of the servers were unreachable and malfunctions, the client should use the rest of the servers
      to complete the download.
    2.check if the file request ever exist. If not, an error message should be returned.
