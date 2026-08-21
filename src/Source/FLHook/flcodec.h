/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 21 августа 2026 06:54:30
 * Version: 1.0.589
 */

#ifndef _FLCODEC_
#define _FLCODEC_

EXPORT bool flc_decode(const char *ifile, const char *ofile);
EXPORT bool flc_encode(const char *ifile, const char *ofile);

#endif