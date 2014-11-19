/**
 * @author      Archit Agarwal <436264@mail.muni.cz>
 * @version     1.0
 * @since       15 Nov 2014
 *
 * This file contains the definitions and API functions required to use Hardware Security Module
 * <p>
 * HSM allows to import two master keys and Personal Identification Number Initialization

 * </p>
 * @see
 *
 *
 */

#include <stdio.h>


/**
 * @brief delete_keys
 *
 *  Deleting Keys
 * <p>
 * This api is used for loading keys
 * </p>
 *
 * @param  type_user Type of user (Only Admin user can perform this operation)
 * @param  pin
 * @param  session_id
 * @return flag specifying success or failure
 */
bool delete_keys(_In_ char *type_user, _In_ char *pin[10],_In_ int session_id)
{

}


