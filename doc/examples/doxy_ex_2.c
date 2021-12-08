/**
 *  See if a timer has expired.
 *
 *  @param systmr       Pointer to a timer object
 *
 *  @retval true        timer expired
 *  @retval false       timer not expired or timer stopped
 */
bool px_systmr_has_expired(px_systmr_t * systmr);
