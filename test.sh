python_commands=(
    './mypython testcases/in01.py'
    './mypython testcases/in02.py'
    './mypython testcases/in03.py'
    './mypython testcases/in04.py'
    './mypython testcases/in05.py'
    './mypython testcases/in06.py'
    './mypython testcases/in07.py'
    './mypython testcases/in08.py'
    './mypython testcases/in09.py'
    './mypython testcases/in10.py'  # Add the new test case here
    './mypython testcases/in11.py'
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
