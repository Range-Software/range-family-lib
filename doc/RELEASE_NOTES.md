## Version 1.2.0

### Improvements

- The strings of the library are translated to Czech, German, Spanish, French,
  Italian, Polish and Ukrainian in addition to English and Slovak. Translation
  files of all languages are part of the library and are kept up to date with
  its strings.
- The element descriptions used in file parsing messages (`tree`, `version`,
  `persons`, `relations`, `person`, `relation`) and the `fuzzy` marker appended
  to a printed date are translated as well. The element names used to read and
  write the files are unchanged, so files stay readable in every language.
- Dates can leave out the day, or both the day and the month. `FDate::isValid()`
  accepts them, `FDate::getPrecision()` tells which parts are set,
  `FDate::toDate()` returns the earliest possible day and the new
  `FDate::toLatestDate()` the latest one. `FDate::toString()` prints only the
  parts which are set and the new `FDate::toLocaleString()` does the same in the
  format of the current locale. The file formats are unchanged; the description
  of the JSON format says that `day` and `month` are optional.
- New `FDateEstimator` estimates birth and death dates of a person. It narrows
  the possible range of every date in the whole tree, using sex specific limits
  of parental age, dates of partners, deaths of parents, siblings and relation
  dates, and then picks the most likely date from typical ages. Fuzzy dates of
  relatives are used with a tolerance. Each result is an `FDateEstimate` with a
  status (unknown, known, estimated, conflict), the date and its limits.

### API changes

- `FTree::findPersonBirthAndDeathDates()` returns two `FDateEstimate` objects
  instead of two `FDate` objects.
- `FConstants::maxReproductiveAge` is replaced by `maxReproductiveAgeFemale`
  (50) and `maxReproductiveAgeMale` (75). `FConstants::minMarriageAge` (15) is
  new and `FConstants::maxAge` is raised from 100 to 110.

### Bug fixes

- `FTree::findPersonBirthAndDeathDates()` replaced a known death date with an
  estimate when the birth date was missing or fuzzy, returned the upper limit as
  the death date of a person known only by the parents, returned the lower limit
  as the birth date of a person known only by a sibling, and averaged
  contradicting limits without reporting them.
- `FDate::merge()` replaced a date with a missing part by the other date even
  when that one was empty. The more precise of the two dates is kept now.

---

## Version 1.1.0

### Improvements

- Person pictures can be stored in separate image files instead of being
  embedded in the family tree file. `FPicture` gained a `url` field, which is
  serialised as plain text in both JSON and XML, along with `getUrl()` and
  `setUrl()` accessors. The url also participates in `FPicture::operator ==()`
  and `FPicture::isEmpty()`.
- Picture urls are stored relative to the family tree file, so a tree and its
  images stay portable when the whole directory is moved or copied. On load
  `FTree` resolves them against the tree file directory and reads the image
  content from the referenced files; on save it writes the images next to the
  tree file, creating intermediate directories as needed, and omits the
  embedded `base64Data`. In memory the urls are kept absolute.
- Only urls resolving to a file at or below the tree file directory are
  honoured. A url pointing elsewhere (a different drive or a path outside the
  tree) is dropped and the image is embedded as `base64Data` instead, so no
  picture is ever lost. The same fallback applies when a referenced file can
  not be read or written.
- Added `FTree::rebasePictureUrls()` to re-point picture urls at the files
  written next to a tree which has been saved under a new name.
- Saving a tree under a different file path moves its picture files along with
  it: the images are written next to the new tree file and the files they were
  stored in before are removed. Pictures named after the tree file (see
  `FTree::pictureFilePath()`) are renamed to follow the new tree file name,
  every other picture keeps its file name and sub-directory.
- Added `FTree::pictureFilePath()` returning the path of the picture file
  belonging to a person next to a given tree file
  (`<tree base name>_<person id>.png`). It is the single definition of the
  picture file naming convention, used both when a picture is attached and when
  a tree is saved under a new name.
- Added `FPicture::readLocalFileData()` and `FPicture::writeLocalFileData()`
  helpers performing the image file I/O. Both require an absolute url;
  relative urls are rejected because they can only be resolved against the
  family tree file, which `FPicture` does not know.

### API changes

- `FPicture` full constructor takes a url:
  `FPicture(title, description, url, data)`.

### Bug fixes

- `FPicture::writeXmlElement()` was writing an empty `base64Data` element for
  pictures without data. It is now omitted, matching `FPicture::toJson()`.

---

## Version 1.0.0

### Bug fixes

- `FTree::updatePerson()` was calling `storedPerson.update(storedPerson)` instead of
  `storedPerson.update(person)`, making the method a no-op.
- `FTree::importMissing()` was iterating `this->relations` instead of
  `pTree->relations`, so relations were never imported from the source tree.
- `FPerson::update()` was not updating the `text` and `picture` fields.
- `FRelation::validate()` was reporting a partner UUID in the duplicate-child
  error message instead of the child UUID, which could also cause an
  out-of-bounds access when the relation has more children than partners.
- `FGeoCoordinate::merge()` condition was inverted: it replaced the existing
  coordinate whenever it was valid (almost always) instead of only when it
  was empty.
