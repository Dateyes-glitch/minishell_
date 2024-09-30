#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

git add .

echo -e "${YELLOW}Files added :).${RESET}"

if [ -z "$1" ]
then
  echo -e "${RED}Please enter your commit msg: ${RESET}"
  read commit_message
  while [ -z "$commit_message" ]
  do
    echo -e "${RED}Again no commit msg: ${RESET}"
    read commit_message
  done

  git commit -m "commit_message"
else
  git commit -m "$1"
echo -e "{YELLOW}Changes commited :)${RESET}!"
fi

git push
echo -e "${GREEN}Changes have been pushed to the remote repo!"
