Executable      =   run_copy.sh
universe        =   vanilla
transfer_input_files = run-template.mac
request_cpus    =   1
request_memory  =   2GB
environment = "JOB_ID=$(Item)"
error = error.err
output = output.out
log = log.log

queue from seq  1 100 200 |
