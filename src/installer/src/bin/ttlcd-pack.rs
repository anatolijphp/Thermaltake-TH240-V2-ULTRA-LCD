//! Host tool (Linux or Windows): appends the application folder to the setup stub.
//!
//!   ttlcd-pack <stub TTLcdStudio-Setup.exe> <app dir> <out.exe>
//!
//! Packs every file under <app dir> (recursively, relative '/' paths) in the format described in src/pack.rs.
//! A stub that already carries a payload is cut back to the bare stub first.
use std::io::Cursor;
use std::path::Path;
use std::process::ExitCode;
use ttlcd_setup::pack;

fn run(stub: &Path, dir: &Path, out: &Path) -> std::io::Result<String> {
    let mut stub_bytes = std::fs::read(stub)?;
    if let Some(p) = pack::read_payload(&mut Cursor::new(&stub_bytes))? {
        stub_bytes.truncate(p.start as usize);
    }
    if !dir.is_dir() {
        return Err(std::io::Error::new(std::io::ErrorKind::NotFound, format!("{} is not a folder", dir.display())));
    }
    let files = pack::collect_dir(dir)?;
    if files.is_empty() {
        return Err(std::io::Error::other(format!("{} contains no files", dir.display())));
    }
    let raw: usize = files.iter().map(|f| f.1.len()).sum();
    let packed = pack::pack(&stub_bytes, &files)?;
    // check what was written before calling it done
    let mut r = Cursor::new(&packed);
    let p = pack::read_payload(&mut r)?.ok_or_else(|| std::io::Error::other("payload not found after packing"))?;
    for e in &p.entries {
        pack::extract(&mut r, &p, e)?;
    }
    std::fs::write(out, &packed)?;
    Ok(format!(
        "packed {} files ({:.1} MB) into {} ({:.1} MB, stub {} bytes)",
        files.len(),
        raw as f64 / 1048576.0,
        out.display(),
        packed.len() as f64 / 1048576.0,
        stub_bytes.len()
    ))
}

fn main() -> ExitCode {
    let a: Vec<String> = std::env::args().skip(1).collect();
    if a.len() != 3 {
        eprintln!("usage: ttlcd-pack <stub TTLcdStudio-Setup.exe> <app dir> <out.exe>");
        return ExitCode::from(2);
    }
    match run(Path::new(&a[0]), Path::new(&a[1]), Path::new(&a[2])) {
        Ok(msg) => {
            println!("{msg}");
            ExitCode::SUCCESS
        }
        Err(e) => {
            eprintln!("ttlcd-pack: {e}");
            ExitCode::FAILURE
        }
    }
}
