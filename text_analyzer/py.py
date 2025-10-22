def get_lines_len(file) -> int:
    return len(file)


def get_characters_len(file) -> int:
    return sum(len(i) for i in file)


def get_empty_lines_len(file) -> int:
    return sum(1 for i in file if not i.strip())


def get_freq_map(file) -> map:
    freq_map = {}
    
    for line in file:
        for char in line:
            if (char.strip()): 
                if char in freq_map:
                    freq_map[char] += 1
                else:
                    freq_map[char] = 1

    return freq_map



if __name__ == "__main__":
    path = input("Введите адрес файла:\n")
    choices = input("Введите номера задач:\n1 - Количество строк в файле;\n2 - Количество символов в файле;\n3 - Количество пустых строк;\n4 - Частотный словарь символов.\n")

    try:
        with open(path, 'r') as file:
            file_content = file.readlines()

            if '1' in choices:
                lines_len = get_lines_len(file_content)
                print(f"\nКоличество строк в файле: {lines_len}") 

            if '2' in choices:
                char_len = get_characters_len(file_content)
                print(f"\nКоличество символов в файле: {char_len}") 
            
            if '3' in choices:
                empty_lines_len = get_empty_lines_len(file_content)
                print(f"\nКоличество пустых строк: {empty_lines_len}") 
            
            if '4' in choices:
                freq_map = get_freq_map(file_content)
                print(f"\nСловарь частотности: {freq_map}") 
    except:
        print(f"Не удалось открыть файл {path}") 