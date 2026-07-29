/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 29 июля 2026 15:46:49
 * Version: 1.0.566
 */

#ifndef _FLCODEC_
#define _FLCODEC_

EXPORT bool flc_decode(const char *ifile, const char *ofile);
EXPORT bool flc_encode(const char *ifile, const char *ofile);

#endif