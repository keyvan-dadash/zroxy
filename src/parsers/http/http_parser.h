/**
 * @file http_parser.h
 */

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <memory.h>

#include "defines.h"
#include "utils/algorithms/string_comp.h"
#include "parsers/http/detail/picohttpparser.h"

/**
 * Holds HTTP Header data's.
 */
struct http_header
{
  /**
   * Name of the header(aka key).
   */
  const char *name;

  /**
   * Lenght of the name.
   */
  size_t name_len;

  /**
   * Value of the name in the header.
   */
  const char *value;

  /**
   * Lenght of the value.
   */
  size_t value_len;
};

/**
 * Holds all HTTP headers.
 */
struct http_headers
{
  /**
   * Array of headers.
   */
  struct phr_header headers[MAX_NUM_HEADERS];

  /**
   * Number of headers that present in HTTP Request.
   */
  size_t number_of_actual_headers;
};

/**
 * HTTP common details.
 */
struct http_common_details
{
  /**
   * HTTP buffer.
   */
  const char *http_buffer;

  /**
   * Size of the HTTP buffer.
   */
  u_int32_t size_of_http_buffer;

  /**
   * Maximum size of HTTP buffer.
   */
  u_int64_t max_buffer_size;

  /**
   * Headers of the HTTP request or response.
   */
  struct http_headers headers;
};

/**
 * HTTP Request details.
 */
struct http_request_details
{
   /**
   * Common HTTP attributes.
   */
  struct http_common_details http_commons;

  /**
   * Method of the HTTP Request.
   */
  const char *method;

  /**
   * Method lenght.
   */
  size_t method_len;

  /**
   * Path of the HTTP Request.
   */
  const char *path;

  /**
   * Path lenght.
   */
  size_t path_len;

  /**
   * HTTP version.
   */
  int minor_version;
};

/**
 * HTTP Response details.
 */
struct http_response_details
{
  /**
   * Common HTTP attributes
   */
  struct http_common_details http_commons;

  /**
   * HTTP version.
   */
  int minor_version;

  /**
   * Status of the HTTP Response.
   */
  int status;

  /**
   * Message of the HTTP Response.
   */
  const char *msg;

  /**
   * Message lenght.
   */
  size_t msg_len;
};

/**
 * Parse HTTP Request.
 * \param http_request Pointer to the details of the HTTP Request.
 * \return Returns number of bytes consumed if successful, -2 if request is partial, -1 if failed.
 */
FORCE_INLINE int zxy_parse_http_request(struct http_request_details *http_request)
{
  http_request->http_commons.headers.number_of_actual_headers = sizeof(http_request->http_commons.headers.headers) / sizeof(http_request->http_commons.headers.headers[0]);
  return phr_parse_request(http_request->http_commons.http_buffer, http_request->http_commons.size_of_http_buffer, &http_request->method, &http_request->method_len,
      &http_request->path, &http_request->path_len, &http_request->minor_version, http_request->http_commons.headers.headers,
      &http_request->http_commons.headers.number_of_actual_headers, 0);
}

/**
 * Parse HTTP Response.
 * \param http_response Pointer to the details of the HTTP Response.
 * \return Returns number of bytes consumed if successful, -2 if request is partial, -1 if failed.
 */
FORCE_INLINE int zxy_parse_http_response(struct http_response_details *http_response)
{
  http_response->http_commons.headers.number_of_actual_headers = sizeof(http_response->http_commons.headers.headers) / sizeof(http_response->http_commons.headers.headers[0]);
  return phr_parse_response(http_response->http_commons.http_buffer, http_response->http_commons.size_of_http_buffer, &http_response->minor_version, &http_response->status,
      &http_response->msg, &http_response->msg_len, http_response->http_commons.headers.headers,
      &http_response->http_commons.headers.number_of_actual_headers, 0);
}

/**
 * Parse HTTP Headers.
 * \param http_commons Pointer to the details of the HTTP Commons.
 * \return Returns number of bytes consumed if successful, -2 if request is partial, -1 if failed.
 */
FORCE_INLINE int zxy_parse_http_headers(struct http_common_details *http_commons)
{
  http_commons->headers.number_of_actual_headers = sizeof(http_commons->headers.headers) / sizeof(http_commons->headers.headers[0]);
  return phr_parse_headers(http_commons->http_buffer, http_commons->size_of_http_buffer, http_commons->headers.headers,
      &http_commons->headers.number_of_actual_headers, 0);
}

/**
 * Modify HTTP Headers.
 * \param http_detail Pointer to the details of the HTTP Commons.
 * \param modify_headers Pointer to the new HTTP Headers.
 * \return Difference in byte manner between new and old HTTP Headers.
 */
FORCE_INLINE int zxy_modify_http_headers(struct http_common_details *http_detail, struct http_header *modify_headers)
{
  struct phr_header *headers = http_detail->headers.headers;
  int max_headers_cnt = http_detail->headers.number_of_actual_headers > MAX_NUM_HEADERS ? MAX_NUM_HEADERS : http_detail->headers.number_of_actual_headers;
  int diffrence = 0;
  for (int i = 0; i < max_headers_cnt; i++)
  {
    if (fast_compare(headers[i].name, modify_headers->name, modify_headers->name_len))
      continue;
    
    if (headers[i].value_len > modify_headers->value_len)
    {
      int distance = ((char *)headers[i].value) - ((char *)http_detail->http_buffer);
      distance += headers[i].value_len;
      int move_size = http_detail->size_of_http_buffer - distance;
      memcpy((char *)headers[i].value, modify_headers->value, modify_headers->value_len);
      memmove((char *)headers[i].value + modify_headers->value_len,
              headers[i].value + headers[i].value_len,
              move_size);
      headers[i].value_len = modify_headers->value_len;
    }
    else if (http_detail->headers.headers[i].value_len < modify_headers->value_len)
    {
      int distance = ((char *)headers[i].value) - ((char *)http_detail->http_buffer);
      distance += headers[i].value_len;
      int move_size = http_detail->size_of_http_buffer - distance;
      memmove((char *)headers[i].value + modify_headers->value_len,
              headers[i].value + headers[i].value_len,
              move_size);
      memcpy((char *)headers[i].value, modify_headers->value, modify_headers->value_len);
      headers[i].value_len = modify_headers->value_len;
    }
    else
    {
      memcpy((char *)headers[i].value, modify_headers->value, modify_headers->value_len);
      return 0;
    }
    
    diffrence = headers[i].value_len - modify_headers->value_len;
    for (int j = i + 1; j < max_headers_cnt; j++)
    {
      headers[j].name += diffrence;
      headers[i].value += diffrence;
    }
  }

  return diffrence;
}

#endif /* HTTP_PARSER_H */

