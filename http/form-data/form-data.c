

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_MSG_DATA_LEN 2048

typedef struct stHttpRspData
{
    int len;
    char data[MAX_MSG_DATA_LEN];
} HttpRspData;

size_t WriteRspData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = 0;
	int newlen = 0;
	HttpRspData *pRspData = (HttpRspData *)stream;

	written = size * nmemb;
	newlen = pRspData->len + written;

	if (newlen > MAX_MSG_DATA_LEN)
	{
		return 0;
	}
	
	memcpy(pRspData->data + pRspData->len, ptr, written);	
	pRspData->len += written;
  	return written;
}

int HttpPostFile(const char *url, const char *file)
{
	HttpRspData rspData;
	CURL *curl_handle = NULL;
	int ret = 0;
	int http_ret = 0;
	struct curl_slist *headers = NULL;
	
    if (NULL == url || NULL == file)
    {
        return -1;
    }
	
	memset(&rspData, 0, sizeof(HttpRspData));		

	struct curl_httppost *formpost = NULL;  
	struct curl_httppost *lastptr = NULL;  
	struct curl_slist *headerlist = NULL;  

	/* Fill in the file upload field */  
	curl_formadd(&formpost,  
				&lastptr,  
				CURLFORM_COPYNAME, "files",  
				CURLFORM_FILE, file,  
				CURLFORM_END);  

	/* Fill in the filename field */  
	curl_formadd(&formpost,  
				&lastptr,  
				CURLFORM_COPYNAME, "bucket_name",  
				CURLFORM_COPYCONTENTS, "public-bucket",  
				CURLFORM_END);  
	
	/* init the curl session */
	curl_handle = curl_easy_init();
    if (!curl_handle)
    {
        return -1;
    }

	// post req
	curl_easy_setopt(curl_handle, CURLOPT_POST, 1); 

	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);

	//set http header
	headers = curl_slist_append(headers, "Expect:");	
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
	//set body json data
	//curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, pReqData->key_value);

	/* Switch on full protocol/debug output while testing */
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	//关闭head输出
	curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0L); //若启用，会将头文件的信息作为数据流输出

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteRspData);

	/* write the page body to this file handle */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rspData);

	/* get it! */
	http_ret = curl_easy_perform(curl_handle);

	//clear header data
	curl_slist_free_all(headers);
	curl_formfree(formpost);

	/* cleanup curl stuff */
  	curl_easy_cleanup(curl_handle);	

	printf("httpagent HTTP/Post file ret %d.:\r\n", http_ret);	

	printf("http post rsp data = %s. \r\n", rspData.data);

    return 0;
}

#define UPLOAD_HTTP_URL "http://192.168.202.90:6134/common-file-upload/"
#define UPLOAD_FILE_NAME "./test.jpeg"

int main()
{
    HttpPostFile(UPLOAD_HTTP_URL, UPLOAD_FILE_NAME);
    return 0;
}