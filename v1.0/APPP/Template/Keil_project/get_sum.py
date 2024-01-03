def calculate_sums(input_file_path, output_file_path):
    try:
        with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
            sum_all = 0
            for line in input_file:
                # Split the line into individual hex values
                hex_values = line.strip().split()

                # Exclude the last line
                if len(hex_values) > 1:
                    # Calculate sum_line
                    sum_line = sum(int(value, 16) for value in hex_values[7:-1])

                    # Calculate sum_line_toggle
                    sum_line_toggle = ~sum_line & 0xFF

                    # Convert to hexadecimal
                    sum_line_hex = format(sum_line, 'x')
                    sum_line_toggle_hex = format(sum_line_toggle, 'x')

                    # Write the results for each line to the output file
                    output_file.write(f"Line: {line.strip()}\n")
                    output_file.write(f"Sum Line (Hex): {sum_line_hex}\n")
                    output_file.write(f"Sum Line Toggle (Hex): {sum_line_toggle_hex}\n\n")

                    # Add to sum_all
                    sum_all += sum_line

            # Convert sum_all to hexadecimal and write to the output file
            sum_all_hex = format(sum_all, 'x')
            output_file.write(f"Total Sum (Sum All, Hex): {sum_all_hex}\n")

    except IOError as e:
        print(f"Error: {e}")

# Replace 'outfile_from_s19.txt' with the path to your S19 file and 'output.txt' with the desired output file name
calculate_sums('outfile_from_s19.txt', 'output_sum.txt')
