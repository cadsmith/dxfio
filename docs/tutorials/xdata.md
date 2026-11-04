# Extended data

Extended data carries an application's own values on an entity: register the application, attach the values to a line, then read them back from the saved file.

## Register the application {#register}

Each set of extended data names the application it belongs to, and the APPID table registers that name. `Edit::Add` adds the record, and fails when the table holds that name already.

```cpp
DxfIO::Document drawing = DxfIO::Create(DxfIO::Version::R2018);

DxfIO::Tables::Appid application{};
application.Record.Name = "WALLPLAN";
DxfIO::Edit::Add(drawing, application);
```

## Attach the values {#attach}

An entity's `Record.Base.Extended` holds one `Xdata` for each application: its name, then its groups in order. Give each value the type dxfio reads its code back as: `std::string` for 1000, `double` for 1040, `std::int64_t` for 1070.

```cpp
DxfIO::Entities::Line wall{};
wall.End = {.X = 400, .Y = 0, .Z = 0};
wall.Record.Base.Extended.push_back(DxfIO::Xdata{
    .Application = "WALLPLAN",
    .Groups =
        {
            {.Code = 1000, .Value = std::string{"W-12"}},
            {.Code = 1040, .Value = 0.2},
            {.Code = 1070, .Value = std::int64_t{60}},
        },
});
const auto handle = DxfIO::Edit::Add(drawing, wall);
```

## Save {#save}

```cpp
const auto saved = DxfIO::SaveFile(drawing, "xdata.dxf");
if (!saved)
{
  std::println("{}", saved.error().What);
  return 1;
}
```

## Read it back {#read}

`FindEntity` finds the line by the handle `Edit::Add` returned, and `BaseOf` gives the extended data of any entity. Other applications' data may sit beside yours, kept as it was read; `Edit::Same` matches the name as AutoCAD does, without case.

```cpp
const auto loaded = DxfIO::LoadFile("xdata.dxf");
if (!loaded)
{
  std::println("line {}: {}", loaded.error().Line, loaded.error().What);
  return 1;
}
const DxfIO::Entity *found = DxfIO::Edit::FindEntity(loaded->Document, *handle);
for (const DxfIO::Xdata &data : DxfIO::BaseOf(*found).Extended)
{
  if (!DxfIO::Edit::Same(data.Application, "WALLPLAN"))
  {
    continue;
  }
  std::println("{} {}: {} thick, rated {} minutes", DxfIO::TypeOf(*found),
               std::get<std::string>(data.Groups.at(0).Value), std::get<double>(data.Groups.at(1).Value),
               std::get<std::int64_t>(data.Groups.at(2).Value));
}
```

```text [Output]
LINE W-12: 0.2 thick, rated 60 minutes
```
