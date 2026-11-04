import { useMemo } from 'react'
import { entries } from '../docs'
import { headers, kinds } from '../api'
import {
  Command,
  CommandDialog,
  CommandEmpty,
  CommandGroup,
  CommandInput,
  CommandItem,
  CommandList,
} from '@/components/ui/command'
import { link, navigate, reference } from '../route'
import { titleOf } from '../sections'

type Entry = { value: string; label: string; detail: string; keywords: string[]; href: string }

const dxfName = new Map(kinds.map((one) => [one.type.split('::').pop() ?? one.type, one.dxf]))

const declared = (path: string, name: string) => `${reference(path)}#${name}`

const build = () => {
  const pages: Entry[] = entries.flatMap((entry) => [
    {
      value: `page ${entry.section} ${entry.slug}`,
      label: entry.page.title,
      detail: titleOf(entry.section),
      keywords: [entry.section],
      href: link(entry.section, entry.slug),
    },
    ...entry.page.parts.map((one) => ({
      value: `section ${entry.section} ${entry.slug} ${one.id}`,
      label: one.title,
      detail: entry.page.title,
      keywords: [entry.section, entry.page.title],
      href: `${link(entry.section, entry.slug)}#${one.id}`,
    })),
  ])
  const types: Entry[] = headers.flatMap((one) => [
    ...one.structs.map((item) => ({
      value: `type ${one.path} ${item.name}`,
      label: item.name,
      detail: dxfName.get(item.name) ?? one.path,
      keywords: [dxfName.get(item.name) ?? '', one.path, one.namespace],
      href: declared(one.path, item.name),
    })),
    ...one.enums.map((item) => ({
      value: `enum ${one.path} ${item.name}`,
      label: item.name,
      detail: one.path,
      keywords: [one.path, ...item.values.map((value) => value.name)],
      href: declared(one.path, item.name),
    })),
    ...one.aliases.map((item) => ({
      value: `alias ${one.path} ${item.name}`,
      label: item.name,
      detail: one.path,
      keywords: [one.path],
      href: declared(one.path, item.name),
    })),
  ])
  const functions: Entry[] = headers.flatMap((one) =>
    [...new Set(one.functions.map((fn) => fn.name))].map((name) => ({
      value: `function ${one.path} ${name}`,
      label: `${name}()`,
      detail: `${one.namespace}, ${one.path}`,
      keywords: [one.path, one.namespace],
      href: declared(one.path, 'functions'),
    })),
  )
  return { pages, types, functions }
}

type Props = { open: boolean; onOpenChange: (open: boolean) => void }

export default function Search({ open, onOpenChange }: Props) {
  const groups = useMemo(() => build(), [])
  const go = (href: string) => {
    onOpenChange(false)
    navigate(href)
  }
  const list = (heading: string, entries: Entry[]) => (
    <CommandGroup heading={heading}>
      {entries.map((one) => (
        <CommandItem key={one.value} value={one.value} keywords={[one.label, ...one.keywords]} onSelect={() => go(one.href)}>
          <span className="search-label">{one.label}</span>
          <span className="search-detail">{one.detail}</span>
        </CommandItem>
      ))}
    </CommandGroup>
  )
  return (
    <CommandDialog
      open={open}
      onOpenChange={onOpenChange}
      title="Search the documentation"
      description="Find a guide section, a type by its name or its DXF name, or a function."
    >
      <Command>
        <CommandInput placeholder="MText, LWPOLYLINE, SaveFile, code pages…" />
        <CommandList>
          <CommandEmpty>Nothing is called that. Try a DXF name such as CIRCLE.</CommandEmpty>
          {list('Guide and concepts', groups.pages)}
          {list('Types', groups.types)}
          {list('Functions', groups.functions)}
        </CommandList>
      </Command>
    </CommandDialog>
  )
}
