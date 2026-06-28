/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 28 июня 2026 11:44:50
 * Version: 1.0.536
 */

#ifndef _FLCODEC_
#define _FLCODEC_

EXPORT bool flc_decode(const char *ifile, const char *ofile);
EXPORT bool flc_encode(const char *ifile, const char *ofile);

#endif