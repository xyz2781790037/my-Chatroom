#ifndef VERIFY_CODE_H
#define VERIFY_CODE_H

#include <iostream>
#include <ctime>
#include <curl/curl.h>
#include <string.h>
#include <cstdlib>
class verCode{
public:
    std::string verify(std::string useQQ);

private:
    struct UploadStatus
    {
        const char *readptr;
        size_t bytes_left;
    };
    static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);
    std::string createCode();
    int sendEmail(const std::string &to, const std::string &subject, const std::string &body);
};
inline int verCode::sendEmail(const std::string &to, const std::string &subject, const std::string &body)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = nullptr;

    const std::string from_email = "2781790037@qq.com"; // 你的邮箱
    const std::string from_auth = "pywgrzspayntdddc";  // 授权码
    const std::string smtp_server = "smtp.qq.com";

    // 构建完整邮件内容（使用 \r\n 是 SMTP 协议要求）
    std::string full_body = "To: " + to + "\r\n" + "From: " + from_email + "\r\n" + "Subject: " + subject + "\r\n" + "\r\n" + body + "\r\n";

    UploadStatus upload_ctx = {full_body.c_str(), full_body.size()};

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_USERNAME, from_email.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, from_auth.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, ("smtps://" + smtp_server + ":465").c_str());
        curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);//SSL加密

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + from_email + ">").c_str());

        recipients = curl_slist_append(nullptr, ("<" + to + ">").c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients); // 告诉 libcurl 收件人是谁

        // 设置发送数据回调，用于分块发送邮件正文
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            std::cerr << "邮件发送失败: " << curl_easy_strerror(res) << std::endl;
        else
            std::cout << "邮件发送成功！" << std::endl;

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl); // 释放之前用 curl_easy_init() 创建的 CURL 句柄，清理 libcurl 为这个句柄分配的所有资源。
    }

    return static_cast<int>(res);
}
inline size_t verCode::payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
    UploadStatus *upload_ctx = static_cast<UploadStatus *>(userp);
    size_t buffer_size = size * nmemb;

    if (upload_ctx->bytes_left == 0)
        return 0;

    size_t copy_size = (upload_ctx->bytes_left < buffer_size) ? upload_ctx->bytes_left : buffer_size;
    memcpy(ptr, upload_ctx->readptr, copy_size);
    upload_ctx->readptr += copy_size;
    upload_ctx->bytes_left -= copy_size;

    return copy_size;
}
inline std::string verCode::createCode(){
    srand(time(0));
    int randomNum = rand() % 999999 + 1;
    std::string strNum = std::to_string(randomNum);
    int len = strNum.size();
    return std::string(6 - len, '0') + strNum;
}
inline std::string verCode::verify(std::string useQQ){
    std::string code = createCode();
    std::string to = useQQ;
    std::string subject = "【注册验证码】";
    std::string body = "您的验证码是：" + code + "\n有效期5分钟，请勿泄露。";
    std::cout << "验证码为 :" << code << std::endl;
    sendEmail(to, subject, body);
    return code;
}
#endif