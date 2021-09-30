
/*************************************************************************
   LAB 1                                                                

    Edit this file ONLY!

*************************************************************************/



#include "dns.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct _DNS_ENTRY
{
    char* domainName;
    IPADDRESS ip;
    struct _DNS_ENTRY* next;
} DNS_ENTRY;

DNSHandle InitDNS( )
{
    //IMPLEMENT ME =)
    DNS_ENTRY* HashTable; 
    int n = 12839;
    HashTable = (DNS_ENTRY*)calloc(n , sizeof(DNS_ENTRY));
    return (DNSHandle)HashTable;
}

unsigned int Hash(char* domainName) {
    unsigned long hash = 5381;
    int c;    
    while (c = *domainName++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }            
    return hash % 12839;
}

void AddElement(DNSHandle hDNS, IPADDRESS ip, char* domainName) {
    DNS_ENTRY* p;
    unsigned int hashValue = Hash(domainName);
    unsigned int StringLenght = strlen(domainName);
    if (((DNS_ENTRY*)hDNS + hashValue)->domainName == NULL) {
        ((DNS_ENTRY*)hDNS + hashValue)->domainName = (char*)malloc(StringLenght + 1);
        strcpy(((DNS_ENTRY*)hDNS + hashValue)->domainName, domainName);
        ((DNS_ENTRY*)hDNS + hashValue)->ip = ip;
    }
    else {        
        p = (DNS_ENTRY*)malloc(sizeof(DNS_ENTRY));
        p->domainName = (char*)malloc(StringLenght + 1);
        strcpy(p->domainName, domainName);
        p->ip = ip;
        p->next = ((DNS_ENTRY*)hDNS + hashValue)->next;
        ((DNS_ENTRY*)hDNS + hashValue)->next = p;
    }
}

void LoadHostsFile( DNSHandle hDNS, const char* hostsFilePath )
{
    // IMPLEMENT ME =)
    FILE* file;
    errno_t err;
    err = fopen_s(&file, "hosts", "r");
    char domainName[201];
    unsigned int ipbytes[4];
    if(file != 0){
        while (fscanf_s(file, "%d.%d.%d.%d %s",
               &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0], domainName, 200) != EOF ) {
            AddElement(hDNS, ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24, domainName);
        } 
        fclose(file);
    }   
}

IPADDRESS DnsLookUp ( DNSHandle hDNS, const char* hostName )
{
    unsigned int index = Hash(hostName);
    DNS_ENTRY* it = ((DNS_ENTRY*)hDNS + index);
    while (it != NULL && strcmp(it->domainName, hostName) != 0)
    {
        it = it->next;
    }
    if (it != NULL) {
        return it->ip;
    }        
    return INVALID_IP_ADDRESS;
}

void ShutdownDNS( DNSHandle hDNS )
{
    for (int i = 0; i < 12839; ++i) {
        DNS_ENTRY* p = ((DNS_ENTRY*)hDNS + i)->next;
        while (p != NULL) {
            DNS_ENTRY* t = p;
            p = p->next;
            free(t->domainName);                  
            free(t);          
        }
    }
    free((DNS_ENTRY*)hDNS);
}

