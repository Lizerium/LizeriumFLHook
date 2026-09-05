/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 05 сентября 2026 08:38:01
 * Version: 1.0.603
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace PlayerCntlSetup
{
    public interface LogRecorderInterface
    {
        /// <summary>
        /// Add an entry to the diagnostics log.
        /// </summary>
        /// <param name="entry">The log entry</param>
        void AddLog(string entry);
    }
}
