#include "Core/TextEditor.h"
#include "Utils/TextProcessing.h"

#include <stdexcept>
#include <utility>

namespace
{
EditResult toEditResult(TextProcessing::TextStatus status)
{
    using TextProcessing::TextStatus;
    switch (status)
    {
    case TextStatus::Ok:               return EditResult::Ok;
    case TextStatus::EmptyText:        return EditResult::NothingToInsert;
    case TextStatus::InvalidCharacter: return EditResult::InvalidText;
    case TextStatus::ColumnOutOfRange: return EditResult::ColumnOutOfRange;
    }
    return EditResult::InvalidText;
}

EditResult validateReplaceText(const mystring &target, const mystring &replacement)
{
    if (target.empty())
        return EditResult::EmptyTarget;
    if (!TextProcessing::isValidLineText(target) ||
        !TextProcessing::isValidLineText(replacement))
        return EditResult::InvalidText;
    return EditResult::Ok;
}
} // namespace

EditResult TextEditor::insertText(int line, int column, const mystring &text)
{
    const int count = doc_.getLineCount();
    if (line < 1 || line > count + 1)
        return EditResult::LineOutOfRange;

    const bool pastEnd = (line == count + 1);
    const mystring none;
    const mystring &base = pastEnd ? none : doc_.getLine(line);

    mystring updated;
    const TextProcessing::TextStatus status =
        TextProcessing::insertAt(base, column, text, updated);
    if (status != TextProcessing::TextStatus::Ok)
        return toEditResult(status);

    saveSnapshot();

    if (pastEnd)
        doc_.insertLine(line, updated);
    else
        doc_.modifyLine(line, std::move(updated));

    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::insertNewLine(int line, const mystring &text)
{
    if (line < 1 || line > doc_.getLineCount() + 1)
        return EditResult::LineOutOfRange;
    if (!TextProcessing::isValidLineText(text))
        return EditResult::InvalidText;

    saveSnapshot();

    doc_.insertLine(line, text);
    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::replaceOccurrence(const mystring &target, const mystring &replacement,
                                         int occurrence, int line, ReplaceReport *report)
{
    const EditResult valid = validateReplaceText(target, replacement);
    if (valid != EditResult::Ok)
        return valid;
    if (occurrence < 1)
        return EditResult::InvalidOccurrence;
    if (line < 0 || line > doc_.getLineCount())
        return EditResult::LineOutOfRange;
    if (target == replacement)
        return EditResult::NoChange;

    int remaining = occurrence;
    int ln = 0;
    for (const Node *node = doc_.getHead(); node != nullptr; node = node->next)
    {
        ++ln;
        if (line != 0)
        {
            if (ln < line)
                continue;
            if (ln > line)
                break;
        }

        const mystring &text = node->textData;
        int pos = text.find(target, 0);
        while (pos != -1)
        {
            if (--remaining == 0)
            {
                mystring updated;
                if (!TextProcessing::replaceAt(text, pos, target.size(), replacement, updated))
                    return EditResult::NoMatchFound;

                saveSnapshot();

                doc_.modifyLine(ln, std::move(updated));
                files_.markModified();
                if (report != nullptr)
                {
                    report->count = 1;
                    report->line = ln;
                    report->column = pos + 1;
                }
                return EditResult::Ok;
            }
            pos = text.find(target, pos + 1);
        }
    }
    return EditResult::NoMatchFound;
}

EditResult TextEditor::replaceAll(const mystring &target, const mystring &replacement,
                                  int line, ReplaceReport *report)
{
    const EditResult valid = validateReplaceText(target, replacement);
    if (valid != EditResult::Ok)
        return valid;
    if (line < 0 || line > doc_.getLineCount())
        return EditResult::LineOutOfRange;
    if (target == replacement)
        return EditResult::NoChange;

    int total = 0;
    int ln = 0;
    bool snapshotTaken = false;
    for (const Node *node = doc_.getHead(); node != nullptr; node = node->next)
    {
        ++ln;
        if (line != 0)
        {
            if (ln < line)
                continue;
            if (ln > line)
                break;
        }

        mystring updated;
        const int n = TextProcessing::replaceAllInLine(node->textData, target, replacement, updated);
        if (n > 0)
        {
            if (!snapshotTaken)
            {
                saveSnapshot();
                snapshotTaken = true;
            }
            doc_.modifyLine(ln, std::move(updated));
            total += n;
        }
    }

    if (total == 0)
        return EditResult::NoMatchFound;

    files_.markModified();
    if (report != nullptr)
    {
        report->count = total;
        report->line = 0;
        report->column = 0;
    }
    return EditResult::Ok;
}

void TextEditor::modifyLine(int lineNumber, const mystring &newText)
{
    if (lineNumber < 1 || lineNumber > doc_.getLineCount())
        throw std::out_of_range("Invalid line number.");

    saveSnapshot();
    doc_.modifyLine(lineNumber, newText);
    files_.markModified();
}

void TextEditor::deleteLine(int lineNumber)
{
    if (lineNumber < 1 || lineNumber > doc_.getLineCount())
        throw std::out_of_range("Invalid line number.");

    saveSnapshot();
    doc_.deleteLine(lineNumber);
    files_.markModified();
}

void TextEditor::deleteRangeOfLines(int startLine, int endLine)
{
    if (startLine < 1 || endLine > doc_.getLineCount() || startLine > endLine)
        throw std::out_of_range("Invalid line range.");

    saveSnapshot();
    doc_.deleteRangeOfLines(startLine, endLine);
    files_.markModified();
}

void TextEditor::deleteCharacterFromLine(int lineNumber, int charIndex)
{
    if (lineNumber < 1 || lineNumber > doc_.getLineCount())
        throw std::out_of_range("Invalid line number.");

    const mystring &line = doc_.getLine(lineNumber);
    if (charIndex < 0 || charIndex >= line.length())
        throw std::out_of_range("Invalid character index.");

    saveSnapshot();
    doc_.deleteCharacterFromLine(lineNumber, charIndex);
    files_.markModified();
}

void TextEditor::deleteTextFromLine(int lineNumber, int startIndex, int count)
{
    if (lineNumber < 1 || lineNumber > doc_.getLineCount())
        throw std::out_of_range("Invalid line number.");
    if (count <= 0)
        return;

    const mystring &line = doc_.getLine(lineNumber);
    if (startIndex < 0 || startIndex >= line.length())
        throw std::out_of_range("Invalid start index.");

    saveSnapshot();
    doc_.deleteTextFromLine(lineNumber, startIndex, count);
    files_.markModified();
}

bool TextEditor::deleteWord(const mystring &targetWord)
{
    if (targetWord.empty())
        return false;

    ResultArray matches = doc_.searchDocument(targetWord);
    if (matches.size() == 0)
        return false;

    saveSnapshot();
    bool deleted = doc_.deleteWord(targetWord);
    if (deleted)
        files_.markModified();
    return deleted;
}

bool TextEditor::undo()
{
    if (!undoStack_.pop(doc_))
        return false;

    files_.markModified();
    return true;
}

bool TextEditor::canUndo() const
{
    return !undoStack_.isEmpty();
}

void TextEditor::saveSnapshot()
{
    undoStack_.push(doc_);
}

void TextEditor::clearUndo()
{
    undoStack_.clear();
}

int TextEditor::undoCount() const
{
    return undoStack_.size();
}

const char *TextEditor::describe(EditResult r)
{
    switch (r)
    {
    case EditResult::Ok:               return "Success.";
    case EditResult::NothingToInsert:  return "Nothing to insert (the text is empty).";
    case EditResult::InvalidText:      return "Text cannot contain line breaks or NUL characters.";
    case EditResult::LineOutOfRange:   return "Line number is out of range.";
    case EditResult::ColumnOutOfRange: return "Column is out of range.";
    case EditResult::EmptyTarget:      return "The text to replace cannot be empty.";
    case EditResult::InvalidOccurrence:return "Occurrence number must be 1 or greater.";
    case EditResult::NoMatchFound:     return "No matching occurrence was found.";
    case EditResult::NoChange:         return "The replacement is identical to the original text; nothing changed.";
    }
    return "Unknown error.";
}
