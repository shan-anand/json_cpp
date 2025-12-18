#!/usr/bin/python
import json
import random
import string
import os
import sys

def generate_random_value(depth=0):
    if depth > 2:
        return random.choice([True, 42, "leaf"])
    
    # Mix of all JSON types
    choice = random.random()
    if choice < 0.2:
        return {"".join(random.choices(string.ascii_lowercase, k=5)): generate_random_value(depth + 1)}
    elif choice < 0.4:
        return [generate_random_value(depth + 1) for _ in range(2)]
    elif choice < 0.6:
        return "".join(random.choices(string.ascii_letters, k=20))
    elif choice < 0.8:
        return random.randint(-1000, 1000)
    else:
        return random.choice([True, False, None])

def parse_size(size_str):
    """Parse size string like '1kb', '2MB', '1g' into bytes"""
    size_str = size_str.lower().strip()
    
    # Extract number and unit
    import re
    match = re.match(r'^(\d+(?:\.\d+)?)([kmg]?b?)$', size_str)
    if not match:
        # Try parsing as plain number
        try:
            return int(size_str)
        except ValueError:
            raise ValueError(f"Invalid size format: {size_str}")
    
    number, unit = match.groups()
    number = float(number)
    
    # Convert to bytes
    if unit in ['', 'b']:
        return int(number)
    elif unit in ['k', 'kb']:
        return int(number * 1024)
    elif unit in ['m', 'mb']:
        return int(number * 1024 * 1024)
    elif unit in ['g', 'gb']:
        return int(number * 1024 * 1024 * 1024)
    else:
        raise ValueError(f"Unknown unit: {unit}")

def create_json_file(target_size_bytes, filename="test_data.json"):
    print(f"Target: {target_size_bytes / 1024 / 1024:.2f} MB")
    
    with open(filename, "w") as f:
        f.write("[\n") # Start JSON array
        current_size = 2
        
        while current_size < target_size_bytes:
            entry = {
                "id": random.getrandbits(32),
                "data": generate_random_value()
            }
            # Convert single entry to string
            entry_str = json.dumps(entry)
            
            # Add comma if not the first element
            if current_size > 2:
                f.write(",\n")
                current_size += 2
            
            f.write(entry_str)
            current_size += len(entry_str)
            
            # Quick progress update
            if random.random() < 0.01: # Update ~1% of the time
                print(f"Progress: {current_size / 1024 / 1024:.2f} MB", end="\r")

        f.write("\n]") # End JSON array

    final_size = os.path.getsize(filename)
    print(f"\nDone! Final Size: {final_size} bytes")

if __name__ == "__main__":
    if len(sys.argv) < 3 or "--help" in sys.argv or "-h" in sys.argv:
        print("Usage: python3 script.py <size> <filename>")
        print("  <size>: Size in bytes, or with units like 1kb, 2MB, 1g")
        print("  <filename>: Output filename")
        sys.exit(1)
    
    try:
        target_size = parse_size(sys.argv[1])
        filename = sys.argv[2]
        create_json_file(target_size, filename)
        print("Done!")
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
