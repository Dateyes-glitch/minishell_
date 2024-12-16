// else if (input[i] == '"') 
// {
//     i++;  // Skip the opening quote
//     char *buffer = calloc(1, 1);  // Initialize an empty string buffer
    
//     while(input[i] != '\0')  // Continue until the end of the string
//     {
//         if (input[i] == '"' && input[i + 1] == '"')  // Check for escaped quote (double quotes)
//         {
//             // Avoid reallocating if not needed; simply skip past the next quote
//             i += 2;  
//         }
//         else if (input[i] == '"')  // End quote, break out of the loop
//         {
//             i++;  // Skip the closing quote
//             break;  // Exit the loop when encountering the end quote
//         }
//         else  // Regular character
//         {
//             size_t len = strlen(buffer);
//             char *tmp = realloc(buffer, len + 2);  // Reallocate memory for one more character
//             if (!tmp)  // Check if realloc succeeded
//             {
//                 free(buffer);  // Free buffer in case realloc fails
//                 perror("Memory allocation failed");
//                 exit(1);  // Exit or handle the error
//             }
//             buffer = tmp;
//             buffer[len] = input[i];
//             buffer[len + 1] = '\0';
//             i++;
//         }
//     }

//     // Do something with the buffer here, such as printing or returning it

//     free(buffer);  // Don't forget to free the allocated buffer at the end
// }
