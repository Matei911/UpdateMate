import os
import sys
import re
from ebooklib import epub
from bs4 import BeautifulSoup, Tag


def slugify(value):
    """
    Simplify a string into a filesystem-friendly slug.
    """
    value = str(value)
    value = re.sub(r"[^\w\s-]", "", value)
    value = re.sub(r"[-\s]+", "_", value).strip().lower()
    return value


def epub_to_txt(epub_path):
    # Load the EPUB
    book = epub.read_epub(epub_path)

    # Extract metadata
    title_meta = book.get_metadata('DC', 'title')
    title = title_meta[0][0] if title_meta else 'Unknown Title'
    author_meta = book.get_metadata('DC', 'creator')
    authors = [a[0] for a in author_meta] if author_meta else ['Unknown Author']

    # Prepare output file
    filename = f"{slugify(title)}.txt"
    last_line = None
    with open(filename, 'w', encoding='utf-8') as out:
        # Write header
        out.write(f"Title: {title}\n")
        out.write(f"Authors: {', '.join(authors)}\n\n")
        last_line = ''

        # Iterate through spine
        for item_id, _ in book.spine:
            item = book.get_item_with_id(item_id)
            if not isinstance(item, epub.EpubHtml):
                continue

            soup = BeautifulSoup(item.get_content(), 'html.parser')
            # Traverse headings and paragraphs in order
            for element in soup.find_all(['h1','h2','h3','h4','h5','h6','p'], recursive=True):
                text = element.get_text(separator=' ', strip=True)
                if not text or text == last_line:
                    continue
                # If heading, underline and separate
                if re.match(r'h[1-6]', element.name, re.IGNORECASE):
                    out.write(text + "\n")
                    out.write('-' * len(text) + "\n\n")
                    last_line = text
                else:
                    out.write(text + "\n")
                    last_line = text
            # Add a blank line between spine items
            if last_line != '':
                out.write("\n")
                last_line = ''

    print(f"Created single text file '{filename}' containing all chapters.")


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python epub_to_txt_converter.py <input.epub>")
        sys.exit(1)
    epub_path = sys.argv[1]
    if not os.path.isfile(epub_path):
        print(f"Error: EPUB file '{epub_path}' not found.")
        sys.exit(1)

    epub_to_txt(epub_path)