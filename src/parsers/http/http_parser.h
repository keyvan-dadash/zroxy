#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "parsers/http/detail/picohttpparser.h"

struct http_headers
{
  struct phr_header headers[20];
};

struct http_common_details
{
  const char *http_buffer;
  u_int32_t size_of_http_buffer;

  /**
   * HTTP headers
   */
  struct http_headers headers;

};

struct http_request_details
{
   /**
   * Common HTTP attributes
   */
  struct http_common_details http_commons;

  /**
   * Method properties
   */
  const char *method;
  size_t method_len;

  /**
   * Path properties
   */
  const char *path;
  size_t path_len;

  /**
   * HTTP version
   */
  int minor_version;
};

struct http_response_details
{
  /**
   * Common HTTP attributes
   */
  struct http_common_details http_commons;

  /**
   * HTTP version
   */
  int minor_version;

  /**
   * Status
   */
  int status;

  /**
   * Message
   */
  const char *msg;
  size_t msg_len;
};

__attribute__((always_inline)) int zxy_parse_http_request(struct http_request_details *http_request)
{
  size_t num_headers = sizeof(http_request->http_commons.headers.headers) / sizeof(http_request->http_commons.headers.headers[0]);
  return phr_parse_request(http_request->http_commons.http_buffer, http_request->http_commons.size_of_http_buffer, &http_request->method, &http_request->method_len,
      &http_request->path, &http_request->path_len, &http_request->minor_version, http_request->http_commons.headers.headers, &num_headers, 0);
}

__attribute__((always_inline)) int zxy_parse_http_response(struct http_response_details *http_response)
{
  size_t num_headers = sizeof(http_response->http_commons.headers.headers) / sizeof(http_response->http_commons.headers.headers[0]);
  return phr_parse_response(http_response->http_commons.http_buffer, http_response->http_commons.size_of_http_buffer, &http_response->minor_version, &http_response->status,
      &http_response->msg, &http_response->msg_len, http_response->http_commons.headers.headers, &num_headers, 0);
}

__attribute__((always_inline)) int zxy_parse_http_headers(struct http_common_details *http_commons)
{
  size_t num_headers = sizeof(http_commons->headers.headers) / sizeof(http_commons->headers.headers[0]);
  return phr_parse_headers(http_commons->http_buffer, http_commons->size_of_http_buffer, http_commons->headers.headers, &num_headers, 0);
}

#endif /* HTTP_PARSER_H */

