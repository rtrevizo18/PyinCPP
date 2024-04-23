python_commands=(
    './mypython in01.py'
    './mypython in02.py'
    './mypython in03.py'
    './mypython in04.py'
    './mypython in05.py'
    './mypython in06.py'
    './mypython in07.py'
    './mypython in08.py'
    './mypython in09.py'
    './mypython in10.py'  # Add the new test case here
)

# Loop through each Python command
for ((i=0; i<${#python_commands[@]}; i++)); do
    # Run the Python command and redirect output to a temporary file
    ${python_commands[$i]} > output_tmp.txt
    
    # Formulate the expected output file name based on the test case number
    expected_output="testcases/out"
    if [ $((i+1)) -lt 10 ]; then
        expected_output+="0"  # Add leading zero if the test case number is less than 10
    fi
    expected_output+="$((i+1)).txt"
    
    # Compare the output with the corresponding expected output file
    diff output_tmp.txt "$expected_output" > /dev/null
    
    # Check the exit status of diff
    if [ $? -eq 0 ]; then
        echo "Test $((i+1)) Passed"
    else
        echo "Test $((i+1)) Failed"
    fi
    
    # Remove the temporary output file
    rm output_tmp.txt
done
