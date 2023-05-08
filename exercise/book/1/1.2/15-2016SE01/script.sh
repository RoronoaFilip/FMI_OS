  #!/bin/bash
 
  if [[ ! -d "${1}" ]]; then
      echo "Error! File not found" >&2
      exit 1
  fi
 
  #type=$(stat "${1}" --printf '%F' 2> /dev/null)
 
  #if [[ "${type}" != "directory" ]]; then
  #  echo "Error! Not a Directory" >&2
  #  exit 1
  #fi
  
  find "${1}" -type l -printf '%p->%l\n' 2> /dev/null | grep '^.*->$'
